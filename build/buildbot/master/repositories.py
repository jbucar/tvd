from buildbot.steps.source.git import Git

middleware = Git(
    name = 'Download middleware from Git',
    mode='full',
    method='clobber',
    repourl='ssh://gitlab@tvd-git.dmz.tecnologia.lifia.info.unlp.edu.ar:10026/root/middleware.git',
    haltOnFailure=True,
)

thirdparty = Git(
    name = 'Download thirdparty from Git',
    mode='full',
    method='clobber',
    repourl='ssh://gitlab@tvd-git.dmz.tecnologia.lifia.info.unlp.edu.ar:10026/root/thirdparty.git',
    haltOnFailure=True,
)

