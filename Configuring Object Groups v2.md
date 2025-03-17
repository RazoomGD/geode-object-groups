# Configuring object groups (since v2.0.0)

Starting from Object Groups v2.0.0, you can edit groups directly from the editor. 

To start editing groups open build tab and press "settings" button at the bottom right corner of the object menu.
**Group editing mode** will be activated and you will see a panel with the options:

![alt text](<assets/Configuring Object Groups v2/1741481376628_image.png>)

![alt text](<assets/Configuring Object Groups v2/1741484287864_image.png>)

Here is a detailed explanation of each of them:

- **New object** - creates a classic button for a single object. For that you must have at least one object previously selected in editor. If you have more than one object selected, a button will be created for each
- **New group** - creates a new group button. If you have selected objects in editor, they will be put into that group. If there are no selected objects, an empty group will be created.
- **Arrow buttons** - they allow you to move the **focused** button in the current tab back and forth. **Focused** button is the one with the colorful outline around it.

    ![alt text](<assets/Configuring Object Groups v2/1741484407640_image.png>)

- **Save changes** - save the current configuration of the groups to the *save file*. If you want to discard all the changes that you made and didn't save, exit the editor, rejecting all suggestions to save the configuration.
- **Delete button** - delete **focused** button (doesn't matter, group or single object).
- **More options** - show advanced options

todo: finish this text



## Save file 

Object groups stores its configuration in the file called `OGv2_config.json`. It located in the mod configuration directory, which you can open by pressing this button in mod settings:

![alt text](<assets/Configuring Object Groups v2/1741484826921_image.png>)

You typically don't need to access this file. And I **definitely** don't recommend you editing it manually. But if you want to share you configuration with someone or you want to back it up, you may need it. 

If you need the file with the default configuration for some reason, you can find it on [github](https://github.com/RazoomGD/geode-object-groups/blob/v2/resources/OGv2_config_default.json).


***
Thanks for using my mod ❤️
*RaZooM*