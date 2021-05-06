# Latency Tracker Plugin
This plugin extends the functionality of the [Open Ephys GUI](https://github.com/open-ephys/plugin-GUI) to support experiments in microneurography, and track constant latency evoked responses.

## Resources

The Open Ephys team is moving their documentation from [atlassian](https://open-ephys.atlassian.net/wiki/spaces/OEW/pages/491527/Open+Ephys+GUI) to [GitHub](https://open-ephys.github.io/gui-docs/User-Manual/Installing-the-GUI.html), however this has not yet been completed so I would suggest referring to the atlassian documentation for now.
### Getting Started with Open-Ephys and LfpLatency Plugin
#### Step 1: Compiling Open-Ephys
**Pre-requisities: cmake, git, (git bash for Windows), (Virtual Studio 2019 for Windows)**
We recommend compiling on Windows.

	1. Go to a place in your local directory. For ease of communication, this directory will be called ~/home
	2. Open a terminal in ~/home, type git clone https://github.com/open-ephys/plugin-GUI.git
##### For Windows Users:
	3. Navigate to ~home/plugin-GUI/Build via command prompt
	4. Enter the following command: cmake -G "Visual Studio 16 2019" -A x64 ..
	5. In Visual Studio, select the open-ephys.sln file from the Build folder.
	6. Optional: From the Build menu, select Configuration Manager, then select Release
	7. Select Build/Build Solution (or press F6) to build the GUI from source or hit the Debug/Release button to build and run. 
	
***N.B. The Release version runs significantly better than debug, but will take longer.***

##### For Mac Users:
	3. Navigate to ~home/plugin-GUI/Build via command prompt
	4. Enter the following command: cmake -G "Xcode" ..
	5. In Xcode, select the open-ephys code project file.
	6. Set the active scheme to install
	7. Build the current scheme by pressing the play button

##### For Linux Users:
	3. Install the Linux dependencies by running sudo ./install_linux_dependencies.sh in the Resources/Scripts folder.
	4. From the same directory, enter: sudo cp 40-open-ephys.rules /etc/udev/rules.d
	5. Enter: sudo service udev restart on Ubuntu/Debian or sudo udevadm control --reload-rules on Fedora/CentOS. These last two steps will allow your computer to communicate with the Open Ephys acquisition board.
	6. Navigate to ~/home/plugin-GUI/Build via command prompt
	7. Type cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release .. to create the build files.
	8. Type make to build the main application. If this fails, you are probably missing dependencies (see above, and please let us know if any dependencies are missing from the instructions). 

If the above steps are successful, there will be a compiled binary at Build/Debug/open-ephys

#### Step 2: Building the Microneurography Plugin
	1. Return to ~/home
	2. Clone the plugin github here

**If you chose debug above, you must continue with debug with the plugin.**
##### For Windows Users:
	3. Repeat all previous Windows steps for the GUI, except now for the plugin. Except this time, choose the install option on the right hand side:
	Copy all files from ~home/OE-plugin-latency-tracker/Build/Release and paste into ~home/plugin-GUI/Build/Release/plugins

<p align="center">
    <img src="./Resources/vsinstall.png" alt="virtual studio.png" title="Choose the INSTALL option">
</p>

##### For Linux users
	3. Repeat steps 6, 7 and 8 for the plugin.
	4. Copy all files from ~home/OE-plugin-latency-tracker/Build/Release and paste into ~home/plugin-GUI/Build/Release/plugins

##### For Mac users
	3. Repeat steps 3, 4, 5, 6, and 7 for the plugin
	4. Copy all files from ~home/OE-plugin-latency-tracker/Build/Release and paste into ~home/plugin-GUI/Build/Release/plugins

<p align="center">
	<img src="./Resources/macinstall.png" alt="mac xcode.png" title="Set the SCHEME">
</p>


### Walkthrough
You should now find an executable called open-ephys in ~home/GUI-plugin/Build/Release (Or debug if you chose that option.)
When launched, you should see LfpLatency in the list of sinks, in the Processors panel on the left.
<p align="center">
    <img src="./Resources/listofsinks.png" alt="LfpLatency.png" title="Here is where you can find the plugin in the GUI">
</p>

Drag it to the signal chain on the bottom of the screen.
Then press the left tab button, this will launch it into the GUI:
<p align="center">
    <img src="./Resources/tabbutton.png" alt="Tab Button.png" title="The button you need to press to launch the plugin">
</p>

A prompt will display, which allows you to load in a previously used configuration. The file this configuration is stored in is **LastLfpLatencyPluginComponents**

<p align="center">
	<img src="./Resources/loadconfig.png" alt="lc.png" title="Load Config Option">
</p>

This plugin is intened for use alongside the Pulse Pal. However, it can be used without. If not detected, a prompt will display upon starting the plugin, as shown here:
<p align="center">
    <img src="./Resources/pulsepalwarning.png" alt="ppw.png" title="Pulse Pal Warning">
</p>
If not using a Pulse Pal, or using legacy data, please click "Continue without PulsePal"

Visualiser for the plugin:
<p align="center">
    <img src="./Resources/mainui.png" alt="ui.png" title="UI">
</p>

> Trigger Threshold: If exceeded in the trigger channel, a new will be added.

> Subsamples Per Window: Number of samples used per 'track' displayed on the plot. It can be thought of as an inverse 'zoom' scale - As the number of values plotted in a given space increases the figure zooms out.

> Starting Sample: If exceeded by the by a sample, it will be stored as a new peak.

> Image Threshold Values: These are the limits for the colour scale on the plot

There are two additional drop down tabs: Setup and Options

Options:
<p align="center">
	<img src="./Resources/options.png" alt="options.png" title="Options">
</p>

> Trigger Channel and Data Channel: Used to set the corresponding channels on Pulse Pal and Recording Device.

> Colour Style Combination: Used to adjust the graphics displayed in the search window.

Setup:
<p align="center">
	<img src="./Resources/setup.png" alt="setup.png" title="Setup">
</p>

This is where the stimulus voltage and rates of spike tracking can be adjusted.

#### Spike Tracking  
The Threshold Values are used to detect spikes in the search box. If the spike magnitude exceeds the detection threshold, and the Track Spike button has been toggled, the spike will be tracked via the search box, with the search box turning green.

<p align="center">
	<img src="./Resources/table.png" alt="table.png" title="Table">
</p>

The table above can be used to track multiple spikes. Any spikes found will be added to the table. They can be tracked and deleted from the table.

#### Stimulus Voltage  

By default, the Stimulus Voltage is set at 3, while the Maximum Stimulus Voltage is set to 10. If the Stimulus Voltage is increased past 4 volts, a warning prompt will appear, alerting the user.
<p align="center">
    <img src="./Resources/voltagewarning.png" alt="voltage warning.png" title="Voltage Warning">
</p>


## Key binds

The following key binds can be used to adjust settings:

| Key               | Binding                                                                |
|--------------     |------------------------------------------------------------------------|
| Up Arrow          | Increase Starting Sample Slider             |
| Down Arrow        | Decrease Starting Sample Slider             |
| Right Arrow       | Increase Search Box Location                |
| Left Arrow        | Decrease Search Box Location                |
| +                 | Increase Subsamples per Window Value        |
| -                 | Decrease Subsamples per Window Value        |
| Page Up           | Increase High Image Threshold               |
| Page Down         | Decrease High Image Threshold               |
| Home              | Increase Low Image Threshold                |
| End               | Decrease Low Image Threshold                |
| F1/F2/F3/F4		| Track Spikes 1 - 4						  |

All these binds work on a number pad, except the function keys.

## Development
Please note, this plugin is still in development. Any suggestions/questions should be directed to the contributors