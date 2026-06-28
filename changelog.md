# 2.2.1
- Add custom scale for opened and pinned groups
- GroupPinning/Gestures setting now only prevents pinning the group by dragging
- Coloring for custom objects
- Optimize rendering custom objects

# 2.2.0
- Allow the creation of custom objects from a single object
- Add group preview options
  - By hovering over it
  - By holding and dragging in the menu 
- Add simpler way to add custom objects (using standard 'new object' button)
- Allow adding focused objects from group to the tab
- Add 2 more arrow buttons to move objects in menus vertically
- Group deletion now suggests you to return buttons back to the tab
- Increase the maximum amount of extra tabs from 5 to 8
- Add options to save/load objects, groups and tabs to files
- Allow moving objects and groups to other tabs using arrow buttons
- Add a keybind to close all groups

# 2.1.9
- Add 'Append deleted' option

# 2.1.8
- Port to 2.2081
- Small additions/fixes

# 2.1.7
- Option to keep pinned groups between editor re-enters (may be enabled in settings)


# 2.1.2
- Dragging groups no longer breaks with mods that use ImGui (e.g DevTools)
- Opened groups no longer closed when switching to 'Edit Groups' mode
- '<cj>Object Search</c>' feature - jump to the tab and group that contain selected object
- IOS support


# 2.1.0
- Group pinning:
  - <cp>Pinning</c> groups so that they stay on the screen
  - Dragging pinned groups across the screen
- Separate checkboxes for enabling each custom tab
- Many UX improvements:
  - Adding objects to the groups straight from the editor tabs without having to place them in the editor
  - Adding objects to the group with 'shift' key (can be enabled in mod settings)
  - Buttons for adding rows/columns to the group are now always active and add rows/columns on the sides of the group when there are no focused button within the group
  - Adding a button to a tab now takes into account the position of the focused button on the opened page instead of always adding to the beginning of the page
  - Option to automatically extend the group when it is full
  - Other small changes...
- Support for <cy>custom objects</c>:
  - Custom objects in groups
  - Custom objects in any of the editor tabs
- Sharing options:
  - Exporting and importing groups/objects/tabs as <cl>JSON</c>
  - Support for exporting/importing custom objects


# 2.0.0 (a complete mod rewrite)
- Group names
- <cg>Full-fledged in-game group configuration</c>:
  - Creating groups
  - Creating classic object buttons
  - Changing button positions within the build tabs
  - Changing button positions within each group
  - Editing group icons
  - You can find more information about it [here](https://github.com/RazoomGD/geode-object-groups/blob/v2/Configuring%20Object%20Groups%20v2.md)
- Additional <cp>tabs</c>
- Group search (experimental, disabled by default)
- Amazing <cl>default group configuration</c>
- Editing tab icons
- Integration with the tooltips of <cj>Creative Mode</c> by Alphalaneous
- Varied JSON features

# 1.0.*
Older versions of the mod. <cr>Deprecated and unrecommended to use!</c> Upgrade to the latest version.