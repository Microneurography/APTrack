# GitHub Actions for CI/CD

## Workflows

* build-lfplatency.yml
  
  Main workflow to compile the LfpLatency plugin. Uses the docker image built from this [dockerfile](https://github.com/Microneurography/OE-plugin-latency-tracker/blob/containerization/Containerization/Dockerfile) as the environment to compile the plugin.

* build-plugin-gui.yml
  
  Workflow to compile the plugin-gui. Designed to build artifacts for build-lfplatency to use (the `Build\` directory), but terns out that didn't work since building lfplatency needs more dependencies. Consider to remove.

## How to use

Currently all actions are set to on `workflow_dispatch`, means they only could be triggered manually:

### build-lfplatency

1. Open the "Actions" tab in GitHub repo page.
2. On the left click on "Build LfpLatency"
3. On the right click on "Run workflow", you can set which branch to run from.

## Todo
* Improvement of the dockerfile
  * Change the entrypoint directory, [currently](https://github.com/Microneurography/OE-plugin-latency-tracker/commit/a4b791d93087020423bd8afa6a92372d04ddfcda) on `\plugin-GUI\Build\`, need to change to `\`
  * Reduce the image size
    * Remove unnecessary components
  * Automate the process to build the image
    * Have a workflow to build and publish the image for the `build-lfplatency`. 
    * Currently it's under Merle's docker hub, and it needs to be moved to somewhere under Microneurography
* Remove `build-plugin-gui.yml` if really useless.
* Have a MacOS environment 
  * MacOS is available in the action's yml under `runs-on` option)

## Q&A
* Why not only use single script to build both `plugin-GUI` and `LfpLatency`?
  * Building `plugin-GUI` itself on GitHub Actions will use 8+ minutes, which could be reduced if we can find a way to only build part of the `plugin-GUI`, but it's still a big time-usage that won't be necessary to be built every time.
* Why not have a workflow to build the `plugin-GUI` and save the output as an artifacts?
  * The experiment with this approach shows that it's not enough to have the `Build` directory or even the whole `plugin-GUI` directory copied. Some dependencies are installed in the environment and the only way I can get `LfpLatency` compiled is either compiling them in one workflow or use a pre-built docker image as the environment. The docker solution obviously save more time.
