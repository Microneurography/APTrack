# Latency Tracker Plugin
This plugin extends the functionality of the [Open Ephys GUI](https://github.com/open-ephys/plugin-GUI) to support experiments.

## Resources

The Open Ephys team is moving their documentation from [atlassian](https://open-ephys.atlassian.net/wiki/spaces/OEW/pages/491527/Open+Ephys+GUI) to [GitHub](https://open-ephys.github.io/gui-docs/User-Manual/Installing-the-GUI.html), however this has not yet been completed so I would suggest to refer to the atlassian documentation for now.

## Quick start guide

- [Compile the main GUI](https://open-ephys.atlassian.net/wiki/spaces/OEW/pages/491621/Windows). 

>Note that [ZMQ plugins](https://github.com/open-ephys-plugins/ZMQPlugins) and [HDF5 Plugins](https://github.com/open-ephys-plugins/HDF5Plugins) are no longer shipped by default with the code base and be **downloaded**, [built and installed](https://open-ephys.atlassian.net/wiki/spaces/OEW/pages/1259110401/Plugin+CMake+Builds) from their own repositories. Follow the steps in the related pages to do so.

- Clone the plugin repo
- Install [cmake](https://cmake.org/download/)
- Create a folder named `Build` inside the cloned `OE-plugin-latency-tracker`
- [Create the build files](https://open-ephys.atlassian.net/wiki/spaces/OEW/pages/1301643269/Creating+Build+files).
- Compile plugin
