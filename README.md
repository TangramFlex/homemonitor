# Home Monitor System Integration

This project demonstrates how to use Tangram Maker&trade; to design and integrate two systems that are similar but not quite the same. 

We'll show how to design a transform between two software components to enable the fetching and display of sensor values within the system.

Lastly, we'll integrate the generated code into other software using Docker containers and test the transform behavior.

## Getting Started

You'll need a free [**Tangram Maker**](https://tangramflex.com/tangram-maker) account to complete this project. [**Click here**](https://tangramflex.com/tangram-maker) to setup an account. 

As well as [**Docker**](https://www.docker.com/get-started). Download and install **Docker Desktop**.

## Background

Imagine that you have moved into a new house! The previous homeowners had a security system installed and decided to leave the old sensors installed all over - they are wired through the walls after all! They also left the central monitor. There is only one problem, you don't necessarily want to pay the alarm company monthly.

Being an entrepreneurial spirit, you decide to create your own monitoring system, but would still like to leverage all the old hardware that is currently installed in your home, besides the central monitor. In a wave of enthusiasm, you create your central monitor replacement using your favorite language and embedded platform. 

There is only one small problem, some of the sensors give data in a slightly different way than you expected, and you would rather not have to go back and re-work your monitor. Enter Tangram Maker.

## Flex Messages and Transform
1. Go to https://maker.tangramflex.io and log in (*[Create an account here](https://tangramflex.com/tangram-maker)*)
2. Create a team if you haven't already
3. Go to **Flex** in menu on the left 

### Define the Old System State Message

First we'll create a Flex package, file, and message named `OldSystemState`. This message represents sensor data that is output by the existing home monitor system. `OldSystemState` describes the state of the sensors: if motion is detected, if a door is open or closed, and the patio temperature.

![Flex Editor](/img/flex-editor.gif)

1. Click the **New Package** button
2. Enter Package Name: `HomeMonitor::OldState`, select your team and click Create
3. Hover over your package, click the three dots ... and **New File**
4. Enter File Name: `Old.flex`, and click Create
5. Paste the following Flex code beneath the module name (under "module HomeMonitor::OldState")
    ```
    enum Motion int32 {
      Active = 0;
      Inactive = 1;
      SensorError = 2;
    }

    enum Entry int32 {
      Open = 0;
      Closed = 1;
      SensorError = 2;
    }

    message struct OldSystemState {
      livingRoomMotion1: Motion;
      livingRoomMotion2: Motion;
      basementMotion1: int32;
      frontDoorEntry: string;
      garageDoorEntry: Entry;
      patioTemp: int32;
    }
    ```
6. Save the file: Click the gear icon on the top right, or Cmd + S / Ctrl + S. (The yellow dot means there are unsaved changes.)
7. Publish your package: Hover over the package, click the three dots ... and then Publish Package

Notice that the `OldSystemState` message fields are of multiple data types. Some are enums, some are integers.

### Define the New System State Message

Next we'll create a 2nd Flex package, file and message named `NewSystemState`. This message represents how we ***want to receive*** the sensor state data for our own monitoring system. 

1. Click the **New Package** button
2. Enter Package Name: `HomeMonitor::NewState`, select your team and click Create
3. Hover over your package, click the three dots ... and **New File**
4. Enter File Name: `New.flex`, and click Create
5. Paste the following Flex code beneath the module name (under "module HomeMonitor::NewState")
    ```
    message struct NewSystemState {
        livingRoomMotion1: string;
        livingRoomMotion2: string;
        basementMotion1: string;
        frontDoorEntry: string;
        garageDoorEntry: string;
        patioTemp: int32;
    }
    ```
6. Save the file: Click the gear icon on the top right, or Cmd + S / Ctrl + S. (The yellow dot means there are unsaved changes.)
7. Publish your package: Hover over the package, click the three dots ... and then Publish Package


`NewSystemState` is similar to `OldSystemState` except that some fields are strings instead of enums or integers.

### Define the Old-to-New Transform

Next we'll create a 3rd Flex package, file, and this time a transform named `Old2New`. This transform will take the `OldSystemState` message and convert it to a `NewSystemState` message.

1. Click the **New Package** button
2. Enter Package Name: `HomeMonitor::Transform`, select your team and click Create
3. Hover over your package, click the three dots ... and **New File**
4. Enter File Name: `Old2NewState.flex`, and click Create
5. Paste the following Flex code beneath the module name (under "module HomeMonitor::Transform")
    ```
    import HomeMonitor::OldState::v1.Old as old
    import HomeMonitor::NewState::v1.New as new

    transform Old2New(oss : old.OldSystemState) -> new.NewSystemState {

        let newLivingRoomMotion1 : string = match(oss.livingRoomMotion1) {
            old.Motion.Active => "Motion Detected";
            old.Motion.Inactive => "No Motion";
            old.Motion.SensorError => "Sensor Error";
        };

        let newLivingRoomMotion2 : string = match(oss.livingRoomMotion2) {
            old.Motion.Active => "Motion Detected";
            old.Motion.Inactive => "No Motion";
            old.Motion.SensorError => "Sensor Error";
        };

        let newGarageDoorEntry : string = match(oss.garageDoorEntry) {
            old.Entry.Open => "Garage Open";
            old.Entry.Closed => "Garage Closed";
            old.Entry.SensorError => "Sensor Error";
        };

        let newBasementMotion1 : string =
            if (oss.basementMotion1 > 0) {
                "Motion Detected";
            } else if (oss.basementMotion1 == 0) {
                "No Motion";
            } else {
                "Sensor Error";
        };

        new.NewSystemState{
            livingRoomMotion1 = newLivingRoomMotion1;
            livingRoomMotion2 = newLivingRoomMotion2;
            basementMotion1 = newBasementMotion1;
            frontDoorEntry = oss.frontDoorEntry;
            garageDoorEntry = newGarageDoorEntry;
            patioTemp = oss.patioTemp;
        };
    }
    ```
6. Save the file: Click the gear icon on the top right, or Cmd + S / Ctrl + S. (The yellow dot means there are unsaved changes.)
7. Publish your package: Hover over the package, click the three dots ... and then Publish Package

In this Flex file, we are importing the ***published packages*** created previously: `HomeMonitor::OldState::v1.Old` and `HomeMonitor::NewState::v1.New`. This allows you to work with their messages. 

A Flex **transform** is a function that takes a message and returns a message, with no side effects. Our `Old2New` transform inputs `OldSystemState` and outputs `NewSystemState`.

The `let` statements define variables within our transform. We're using pattern matching to map the `Motion` and `Entry` enums into strings. And we're using a conditional if/else statement to map the basementMotion1 integer into a string. It turns out that for the existing home monitor system, when basementMotion1 is greater than zero that means motion is detected, when it's equal to 0 then no motion is detected, and if it's less than zero there's a sensor error.

The last statement of a transform is what's returned/output. In this case `NewSystemState` is output.

For more information about Flex code syntax, see [Flex Docs](https://docs.tangramflex.io/docs/flex/start).

## System Design & Code Gen

Now that our Flex messages and transform are published, let's design the system and generate code. 

Next we'll create a top-level component to house our system. In it we'll create 2 components, one for each message.

**Workspace Component:**

1. Exit the Flex Editor by clicking **Components** in the left menu
2. Click **New Component** 
3. Enter Component Name `HomeMonitorSystem`, select your team, and click Create.
4. Click **Add Implementation**, enter `v1`, and click Create
5. Click **Open Workspace**

![Workspace Component](/img/workspace-component.png)

**Sensor Reporter Component:**
1. Within the workspace, click **Components** on the left menu (the box icon)
2. Create a new Component named `SensorReporter`
3. Add a port with the following options:
   - Message Set: `HomeMonitor::OldState::v1`
   - Message: `OldSystemState`
   - Direction: `Out`
4. Click Next, enter Implementation Name: `v1` and click Create
5. Drag it into the workspace

**Sensor Monitor Component:**
1. Create another new Component named `SensorMonitor`
2. Add a port with the following options:
   - Message Set: `HomeMonitor::NewState::v1`
   - Message: `NewSystemState`
   - Direction: `In`
3. Click Next, enter Implementation Name: `v1` and click Create
4. Drag it into the workspace

**Connect Components:**

1. Click the connection icon in the `SensorReporter` component, then the same icon in the `SensorMonitor`
2. Click **New Connection** and select the messages you created in the drop downs
3. Click **Done**

**Generate Code:**

1. Click **Workflows** on the left menu (the list icon)
2. Click **New Workflow**, enter name `Transform Code` and click Create.
3. Select it and click **+ Tasks**
4. Drag the `Transform: ...` block into the right area
5. Click **Close** and then **Run**. Now is a good time to refill your coffee. This can take about 10 minutes to generate the component software interfaces and compile the transform application.
6. Download the artifact when it's complete by clicking the disk icon
7. Untar it. You should have a folder named `out`


## Integrate and Test

Next we're going to utilize Docker and some wrapper code to interact with the generated code in a Python shell.

**Clone This Repo**

1. Clone (or download) this repository to your computer
2. Within your cloned repo, create a new folder called `code-gen` 
3. Move the `out` folder (that Tangram Maker generated) into the `code-gen` folder. Your files should look simlar to this:
    ```
    ├── code-gen
    │   └── out
    │       ├── newstate
    │       ├── oldstate
    │       └── transform
    ├── Dockerfile
    ├── exercise.py
    ├── Makefile
    └── transform_msg_wrapper.cpp
    ```

**Run Docker**    
1. Start the **Docker Desktop** app on your computer
2. Open **Terminal** and enter these commands
    ```
    cd homemonitor
    make shell-docker
    ```
    *If you get "Cannot connect to the Docker daemon at unix:///var/run/docker.sock. Is the docker daemon running?" make sure Docker Desktop is running and try again.*

This will compile both message sets, as well as the transform code you placed in your `code-gen` folder, producing a library file. Additionally it will compile C wrapper code necessary to run your code in Python. It will then spin up a Python shell for you to exercise your new library. All of the this happens in Docker without having to worry about glibc versions or other dependencies installed on your machine. Yay containers :tada:


**Test Different Values**

When the Python shell starts up it automatically outputs a transformed message body.

Explore what happens by changing the old system state values and re-running the transform. You can reassign a value to any of the variables, like `basement_motion_1 `, and then transform it again to see the new values. Your output will match the strings in the transform you made.

For example, to see the transformed basement sensor error enter:

```
basement_motion_1 = -1

print("New Message Body:", old2new(living_room_motion_1, living_room_motion_2, garage_door_entry, front_door_entry, basement_motion_1, patio_temp))
```

Another example would be to change the value of both living room motion sensors:
```
living_room_motion_1 = Motion.Active
living_room_motion_2 = Motion.Inactive

print("New Message Body:", old2new(living_room_motion_1, living_room_motion_2, garage_door_entry, front_door_entry, basement_motion_1, patio_temp))
```

Or to test the garage door sensor value:
```
garage_door_entry = Entry.Open

print("New Message Body:", old2new(living_room_motion_1, living_room_motion_2, garage_door_entry, front_door_entry, basement_motion_1, patio_temp))
```

Looking back at the transform we created, all the possible values to try would be:
- `living_room_motion_1` = 
  - Motion.Active
  - Motion.Inactive
  - Motion.SensorError
- `living_room_motion_2` = 
  - Motion.Active
  - Motion.Inactive
  - Motion.SensorError
- `garage_door_entry` = 
  - Entry.Open
  - Entry.Closed
  - Entry.SensorError
- `basement_motion_1` = 
  - 1 (or any positive integer)
  - 0
  - -1 (or any negative integer)
- `front_door_entry` wasn't modified, this can be any string.
- `patio_temp` wasn't modifier either, this can be any integer

Note, if you prefer to not interact with Python in the shell directly, you can just modify the `exercise.py` file, run `docker-compose up --build` and see the output.

**Exiting**

To exit the Python shell enter command `exit()`.

## Summary

In this project you've used Tangram Maker to design and integrate two systems that are similar but not quite the same. 

You've designed a transform between two software components to enable the fetching and display of sensor values within the system.

Lastly, you've integrated the generated code into other software (Python) using Docker containers and tested the transform behavior.

For additional documentation on Tangram Maker, check out https://docs.tangramflex.io/ and our community forum https://discuss.tangramflex.io/.
