#!/usr/bin/env python
# -*- coding: latin-1 -*-
# -*- python -*-
# ex: set syntax=python:

from config import config

from buildbot.status.builder import Results
from buildbot.status.mail    import MailNotifier

def simple_message_formatter(mode, name, build, results, master_status):
    result = Results[results]

    text = list()
    text.append("STATUS: %s" % result.title())
    text.append("URL: %s" % master_status.getURLForThing(build))
    for ss in build.getSourceStamps():
        if ss.branch:
            text.append("BRANCH: %s" % ss.branch)

    return {
            'body' : "\n".join(text),
            'type' : 'plain'
    }

def configure(buildmasterConfig):
    mailConf = config["mail"]
    cmailer = MailNotifier(
        mode = ('failing','exception'),
        fromaddr = mailConf["fromaddr"],
        sendToInterestedUsers = mailConf["sendToInterestedUsers"],
        extraRecipients = mailConf["extraRecipients"],
        useTls = mailConf["useTls"],
        relayhost = mailConf["relayhost"],
        smtpPort = mailConf["smtpPort"],
        smtpUser = mailConf["smtpUser"],
        smtpPassword = mailConf["smtpPassword"],
        messageFormatter = simple_message_formatter
    )
    buildmasterConfig['status'].append(cmailer)
