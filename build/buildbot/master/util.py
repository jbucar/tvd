#!/usr/bin/env python
# -*- coding: latin-1 -*-
# -*- python -*-
# ex: set syntax=python:

import tarfile
import tempfile
import datetime
import json
import os
import lazy_reload
import hashlib
import shutil
import re
import sys
import bz2
import urllib

from zope.interface import implements
from buildbot.interfaces import IRenderable
from buildbot.process.properties import Interpolate
from buildbot.schedulers.forcesched import StringParameter

import twisted.python

class EmptyStringParameter(StringParameter):

    def getFromKwargs(self, kwargs):
        """Simple customization point for child classes that do not need the other
           parameters supplied to updateFromKwargs. Return the value for the property
           named 'self.name'.

           The default implementation converts from a list of items, validates using
           the optional regex field and calls 'parse_from_args' for the final conversion.
        """
        args = kwargs.get(self.fullName, [])

        if len(args) == 0:
            if self.required:
                raise ValidationError("'%s' needs to be specified" % (self.label))
            if self.multiple:
                args = self.default
            else:
                args = [self.default]

        if self.regex:
            for arg in args:
                if not self.regex.match(arg):
                    raise ValidationError("%s:'%s' does not match pattern '%s'"
                                          % (self.label, arg, self.regex.pattern))

        try:
            arg = self.parse_from_args(args)
        except Exception, e:
            # an exception will just display an alert in the web UI
            # also log the exception
            if self.debug:
                traceback.print_exc()
            raise e
        if arg is None:
            raise ValidationError("need %s: no default provided by config"
                                  % (self.fullName,))
        return arg


class ReInterpolate(Interpolate, object):

    implements(IRenderable)

    def __init__(self, interpolate, *args, **kwargs):
        self.interpolate = interpolate
        Interpolate.__init__(self, 'ReInterpolate("%s")' % interpolate.fmtstring, *args, **kwargs)

    def getRenderingFor(self, props):
        self.fmtstring =  getattr(self.interpolate.getRenderingFor(props), 'result', '')
        twisted.python.log.msg("ReInterpolate fmtstring: '%s'" % self.fmtstring )
        if not self.args:
            self.interpolations = {}
            self._parse(self.fmtstring)
        return Interpolate.getRenderingFor(self, props)

def temporary_file(props, log, location, content):
    tmp_location = props['workdir'] + '/' + location
    config = tempfile.NamedTemporaryFile(dir=tmp_location, delete=False)
    log.append('location: %s' % config.name)
    log.append('content: %s' % content)
    config.write(content)
    res = config.name
    config.close()
    return res

def find_in_file(src, to_find):
    src_file = os.fdopen(os.open(src, os.O_RDONLY))
    line = [line for line in src_file if line.find(to_find) > 0][0]
    line = line[line.find(to_find) + len(to_find):]
    return line.strip(' \t\n"')

def now():
    return datetime.datetime.now().strftime('%Y%m%d%H%M')

def format_commit_number(log, props):
    log.append('Out:' + props['commit'])

    first = props['commit'].find('-')
    if (first > 0):
        last = props['commit'].rfind('-')
        props['commit'] = props['commit'][first+1:last]

    return

def reload_all(dir, log, success, props):
    log.append("reload all in %s directory" % dir)
    if os.path.exists(dir):
        files = os.listdir(dir)
        suffix = ".py"

        for file in files:
            if file.endswith(suffix):
                module = file[:len(file) - len(suffix)]
                lazy_reload.lazy_reload(module)
                log.append("reloading module %s from file %s" % (module, file) )

        success = True
    else:
        log.append("%s not exists" % dir)
        success = False

def set(log, to_set):
    log.append("to set: %s" % to_set)
    return to_set

def remove_files( dir, amount, log ):
    log.append("removing files from %s" % dir)
    if os.path.exists(dir):
        files = os.listdir(dir)
        if len(files) > amount:
            log.append("Files in dir: %s" % "\n".join(files))
            files.sort(key=lambda x: os.stat(os.path.join(dir, x)).st_mtime)
            for file in files[:len(files) - amount]: os.remove(os.path.join(dir, file))
    else:
        log.append("%s not exists" % dir)

def make_symbolic_link(src, dst, log, success):
    src = os.path.expanduser(src)
    dst = os.path.expanduser(dst)
    if not os.path.exists(src):
        log.append("%s not exists" % str(src))
        success = False
    if os.path.exists(dst):
        log.append("removing old " + str(dst))
        os.remove(dst)
    os.symlink(src, dst)

def make_versioned_link(src, dst, linkName, linkExt, version, log, success):
    src = os.path.expanduser(src)
    fixedVersion = '.'.join(version.split('.')[:2])
    dst = os.path.join( os.path.expanduser(dst), '%s-%s.%s' % (linkName, fixedVersion, linkExt) )
    if not os.path.exists(src):
        log.append("%s not exists" % str(src))
        success = False
    if os.path.exists(dst):
        log.append("removing old " + str(dst))
        os.remove(dst)
    os.symlink(src, dst)

def tar(tarball_name, archives, log, props, success):
    dirs = []
    os.chdir(props['workdir'])
    for archive in archives:
        archive = './' + archive
        if os.path.exists(archive):
            log.append('Untar %s ' % archive)
            tarball = tarfile.open(archive, 'r')
            tarball.extractall()
            dirNames = tarball.getnames()

            dir = [dirName for dirName in dirNames if dirName.find("firmware") > 0]
            if not dir:
                log.append("firmware not exists")
                success = False
                return
            dir = dir.pop()
            dir = dir[0:dir.find('firmware')]
            dirs.append(os.path.dirname(dir))

            tarball.close()
        else:
            log.append("Archive %s not exists" % archive)
            success = False
            return;

    tarball = tarfile.open(tarball_name + ".tar.gz", "w:gz")
    for dir in dirs:
        os.chdir(props['workdir'] + '/' + dir)
        tarball.add("firmware")
    tarball.close()
    log.append("tarfile %s created" % tarball_name)
    return

def create_join_config(filename, model_id, props, src, version, local_src, log, output_format='xml', mandatory=None):

    def dic2xml(dic, depth=0):
        xml = ''
        tabs = '\t' * depth

        for key in dic.keys():
            if isinstance(dic[key], dict):
                xml = '%s%s<%s>\n%s</%s>\n' % (xml, tabs, key, dic2xml(dic[key], depth+1), key)
            elif isinstance(dic[key], list):
                xml = '%s%s<%s>\n' % (xml, tabs, key)
                for item in dic[key]:
                    xml = '%s%s%s' % (xml, tabs, dic2xml(item, depth+1))
                xml = '%s%s</%s>\n' % (xml, tabs, key)
            else:
                value = dic[key]
                xml = '%s%s<%s>%s</%s>\n' % (xml, tabs, key, value, key)

        return xml

    def hashfile(afile, hasher, blocksize=65536):
        buf = afile.read(blocksize)
        while len(buf) > 0:
            hasher.update(buf)
            buf = afile.read(blocksize)
        return hasher.hexdigest()

    desc = 'Firmware version %s builded automatically by Buildbot at %s' % (str(version), datetime.datetime.now().strftime('%Y-%m-%d %H:%M'))
    file_to_hash = os.open(local_src, os.O_RDWR)

    root = { 'root': {
        'makerId': '208',
        'modelId': model_id,
        'version': str(version),
        'description': desc,
        'firmware': src,
        'size': str(os.path.getsize(local_src)),
        'md5': hashfile(os.fdopen(file_to_hash, 'rb'), hashlib.md5())
    }}
    if mandatory != None:
        root['root']['mandatory'] = (mandatory == 'True')

    fd = os.open(filename, os.O_RDWR | os.O_CREAT)
    fo = os.fdopen(fd, 'w')
    if output_format=='xml':
        fo.write(dic2xml(root))
    elif output_format=='json':
        fo.write(json.dumps(root['root'], indent=4))
    fo.close()
    log.append(os.path.abspath(filename))

    return

def get_file(location):
    onlyfiles = [ f for f in os.listdir(location) if os.path.isfile(os.path.join(location,f)) ]
    return re.findall(r'htmlshell-.*\.tar.gz', ' '.join(onlyfiles))[0]

def get_tac_img_version(location, log):
    onlyfiles = [ f for f in os.listdir(location) if os.path.isfile(os.path.join(location,f)) ]
    filename = os.path.basename( re.findall(r'image-\w+-\d+.\d+.\d+.tac', ' '.join(onlyfiles))[0] )
    log.append( 'Get tac version from: %s' % filename )
    return '.'.join( filename.split('-')[2].split('.')[:3] )

def get_htmlshell_version(src, log):
    log.append( 'Get htmlshell version from: %s' % src )
    # src = htmlshell-2.2.5013.tar.gz
    return '.'.join(os.path.basename(src).split('-')[1].split('.')[:2])

def get_apk_version(location, log):
    onlyfiles = [ f for f in os.listdir(location) if os.path.isfile(os.path.join(location,f)) ]
    filename = os.path.basename( re.findall(r'android-.*-.*\.apk', ' '.join(onlyfiles))[0] )
    log.append( 'Get tac version from: %s' % filename )
    return '.'.join( filename.split('-')[2].split('.')[:3] )
