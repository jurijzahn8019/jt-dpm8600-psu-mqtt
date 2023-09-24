Import("env")
import os;

print("Current CLI targets", COMMAND_LINE_TARGETS)
print("Current Build targets", BUILD_TARGETS)

def before_buildprog(env):
    print("before_buildprog")
    # do some actions

    os.chdir("app")
    # call Node.JS or other script
    env.Execute("npm i && npm run build")
    os.chdir("..")

# env.AddPreAction("buildprog", before_buildprog)

before_buildprog(env)