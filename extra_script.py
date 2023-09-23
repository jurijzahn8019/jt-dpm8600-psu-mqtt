Import("env")

print("Current CLI targets", COMMAND_LINE_TARGETS)
print("Current Build targets", BUILD_TARGETS)

def before_buildprog(source, target, env):
    print("before_buildprog")
    # do some actions

    # call Node.JS or other script
    env.Execute("sh -c 'cd app; npm run build; cd ..'")

env.AddPreAction("buildprog", before_buildprog)