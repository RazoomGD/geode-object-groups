# Configuring object groups (since v2.0.0)

Starting from Object Groups v2.0.0, you can edit groups directly from the editor. This guide gives a detailed explanation of all the new features.

# Getting started

![alt text](<assets/Configuring Object Groups v2/1741481376628_image.png>)

To start editing groups open build tab and press `settings` button at the bottom right corner of the object menu.
**Group editing mode** will be activated and you will see a panel with the options:

![alt text](<assets/Configuring Object Groups v2/1743481100382_image.png>)

Also you will see the colored frame around one of the buttons. It marks the button that Object Groups is currently working with. In this guide, I will refer to it as **focused button**

![alt text](<assets/Configuring Object Groups v2/1743458920566_image.png>)

Now you can start editing you groups


# Key concepts

Basic concept of working with this mod is the same as working with the RobTop's **custom object system**. This means that in order to create new object or group or add objects to the group you have to select some objects in editor first. 

**One important thing before you start is messing around with options is saving changes:**

Object Groups stores its configuration in the save file (will be explained [later](#save-file)). When you press `Save Changes` button, mod saves all you objects and groups to this file. Once you saved the changes they cannot be discarded. To discard unsaved changes just leave the editor without saving them.

![alt text](<assets/Configuring Object Groups v2/1743462250771_image.png>)

Also you can make a **backups** of you group configuration, so that you don't have to worry about messing it up. Backups are explained [here](#save-file). 

Now you're ready to explore the Object Groups options:

# Options

- **Basic options**
  - [create new object](#creating-objects)
  - [create new group](#creating-groups)
  - [arrange buttons](#arranging-buttons)
  - [delete button](#deleting-buttons)
- **Group options**
  - [rename group](#renaming-groups)
  - [resize group](#resizing-groups)
  - [add/remove object](#addingremoving-objects)
  - [arrange objects](#arranging-objects-within-the-group)
  - [set group icon](#changing-group-icon)
  - [add custom objects](#adding-custom-objects)
- **Advanced options**
  - [create tab icon from selected objects](#create-tab-icon-from-selected-objects)
  - [create new group from layout](#create-new-group-from-layout)
  - [add custom object](#new-custom-object)
  - [copy focused to clipboard](#copy-focused-to-clipboard)
  - [copy tab to clipboard](#copy-tab-to-clipboard)
  - [paste group(s) from clipboard](#paste-groups-from-clipboard)


# Basic options

## Creating objects

![alt text](<assets/Configuring Object Groups v2/1743460705277_image.png>)

To create a new object:
- open the build tab and page where you want to create it
- select one or more objects in editor
- press `New Object` button

If there is a focused button in the opened page new objects will be added starting from its position. Otherwise they will be added to the beginning of the opened page.

![alt text](<assets/Configuring Object Groups v2/1743460416535_image.png>)


## Creating groups

![alt text](<assets/Configuring Object Groups v2/1743460880587_image.png>)

To create a new group select some objects that you want to put in it in editor and press `New Group` button.

To create a new **empty** group deselect all objects in editor and press `New Group` button.

![alt text](<assets/Configuring Object Groups v2/1743461101055_image.png>)

If there is a focused button in the opened page new group will be be placed on its position. Otherwise new group button will be added to the beginning of the opened page.


## Arranging buttons

Select the button within the build tab (not within the group) and use these arrows.

![alt text](<assets/Configuring Object Groups v2/1743461529299_image.png>)


## Deleting buttons

Select the button within the build tab (not within the group) and press that button.

![alt text](<assets/Configuring Object Groups v2/1743462865211_image.png>)

**Very important** thing is that Object Groups restricts the removal of buttons from the tabs where they located in vanilla GD. (This is mainly made in order to prevent the loss of objects after their removal from groups) 

That means that that the button must appear in its original tab either within the group or among the ungrouped objects. Otherwise Object Groups will add it after all other buttons of the tab. Even if you delete the button from the tab it will appear there after the next editor reload.


# Group options

When you open the group you can see a bunch of buttons around it. They are responsible for **group options**

![alt text](<assets/Configuring Object Groups v2/1743463148772_image.png>)


## Renaming groups

Open the group, press `plus` button, enter the name in the appeared popup.

![alt text](<assets/Configuring Object Groups v2/1743463578230_image.png>)

![alt text](<assets/Configuring Object Groups v2/1745981010899_image.png>)


## Resizing groups

You can resize groups by adding and removing rows and columns:

![alt text](<assets/Configuring Object Groups v2/1743475302709_image.png>)

Columns and rows are pasted/deleted relative to the focused button position. If there are no focused button within the group, they will be pasted on the sides of the group (deleted buttons will be inactive).

Each button does exactly what the text on is says. Be careful because `delete` buttons delete row/column together with all buttons in it.


## Adding/removing objects

These two buttons are responsible for adding/removing objects

![alt text](<assets/Configuring Object Groups v2/1743476309288_image.png>)

- To add one ore more objects to the group, select objects that you want to add in editor and press the upper button. If you have the focused button within the group, new objects will be added starting from its position
  - **Important:** starting from Object Groups v2.1.0 it is not necessary to place object in editor to add it to the group. If there is a focused button, that button will be added to the group.
  - **Important 2:** Starting from Object Groups v2.1.0 on PC it is possible to add objects to the group by holding `shift` and clicking on the object button. (this option is disabled by default, so you have to enable this option in settings to use it)

- To remove the button from the group, select it and press the lower button.


## Arranging objects within the group

Select the button and use the arrows on top of the group. Quite straightforward.


## Changing group icon

Group icon is the object that you see on its button.

In Object Groups v2.0.0 group icon can have 1 to 4 objects on it:

![alt text](<assets/Configuring Object Groups v2/1743478174401_image.png>)

To change the icon, select 1 to 4 objects in the editor and press `Set Icon` button in the group options menu:

![alt text](<assets/Configuring Object Groups v2/1743478269845_image.png>)


## Adding custom objects

Creates new custom object and adds it to the group. Custom objects will be covered in details [here](#custom-objects)

# Advanced options

These are the options that are located in `More Options` menu:

![alt text](<assets/Configuring Object Groups v2/1743478508322_image.png>)

![alt text](<assets/Configuring Object Groups v2/1745980592329_image.png>)

These are the advanced options, so you probably won't need to use them. But I explain them for the sake of completeness of the guide:


## Create tab icon from selected objects

This option is related to these icons:

![alt text](<assets/Configuring Object Groups v2/1743478733910_image.png>)

It allows you to change the default icon to whatever you've created in editor:

![alt text](<assets/Configuring Object Groups v2/1741480485078_image.png>)

To reset the tab icon to the default one, deselect objects in editor and press this button


## Create new group from layout

It tries to put the selected objects in a new group while preserving their relative positions. To do this, objects must be arranged in a grid-like manner in the editor (there must be a distinct rows and columns. Alignment to the editor grid is not required). Example:

![alt text](<assets/Configuring Object Groups v2/1743479730695_image.png>)

## New custom object

Creates new custom object and adds it to the tab. Custom objects will be covered in details [here](#custom-objects)


## Copy focused to clipboard

Copies the focused group, object to the clipboard in json format. You can explore the format yourself.

## Copy tab to clipboard

Copies the entire tab to the clipboard in json format.


## Paste group(s) from clipboard

Adds groups from json content of clipboard to the current tab.You can use copy/paste options to share you groups with other creators. Copy-paste system also supports **custom objects**. It copies information about custom objects together with groups and objects


# Custom objects

Science version 2.1.0 Object Groups supports **custom objects** in groups and tabs. Custom objects have darker button background but not really differ from built-in GD custom objects:

![alt text](<assets/Configuring Object Groups v2/1745982251402_image.png>)

You can add new custom object to the group:
- by selecting it in the custom tab and clicking on the button for adding objects
- by clicking `New custom obj.` button in extra group options popup (this will create a new custom object from selected objects and add it to the group): ![alt text](<assets/Configuring Object Groups v2/1745981356235_image.png>)

You can add new custom object to the tab by opening 'More options' menu and clicking `New custom object` button (this will create a new custom object from selected objects and add it to the tab): ![alt text](<assets/Configuring Object Groups v2/1745982520482_image.png>)



# Save file 

Object groups stores its configuration in the file called `OGv2_config.json`. It located in the mod configuration directory, which you can open by pressing this button in mod settings:

![alt text](<assets/Configuring Object Groups v2/1741484826921_image.png>)

You typically don't need to access this file. And I **definitely** don't recommend you editing it manually. But if you want to share you configuration with someone or you want to back it up, you may need it. 

If you need the file with the default configuration for some reason, you can find it on [github](https://github.com/RazoomGD/geode-object-groups/blob/v2/resources/OGv2_config_default.json).


# Conclusion

This was pretty much all the information about configuring groups. I have not covered all the functionality of the mod here (including extra tabs, group search and all the settings). But I hope it's not very difficult to figure it out. There are also many info buttons everywhere which can help you!

![alt text](<assets/Configuring Object Groups v2/1743480603262_image.png>)


***
Thanks for using my mod ❤️
*RaZooM*