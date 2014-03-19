HansoftCloneProjectClientPlugin
===============================

About this program
------------------
This program is a Hansoft SDK client plugin that adds functionality to the Hansoft client. Note that to use this
program you need to have the SDK option enabled on your Hansoft Server.

This plugin adds a menu item to the right-click menu in Hansoft called "Create project from template". Under this menu there
will be submenu-items for each of the projects in the current database, provided that the project name starts with "Template - " .
When selecting a project menu you will be asked to provide a name for a new project which then will be created using the
selected project as a template. The following information will be copied to the new project from the selected template
project:

* Custom columns in the schedule, product backlog, and QA section
* View presets in the schedule, product backlog, and QA section
* Reports in the schedule, product backlog, and QA section.
* Pipelines and workflows
* The basic information and structure of tasks in the schedule and the product backlog, including any attached documents.

NOTE: The ownership of reports will be changed to the user that runs the command. If this user is not intended to be a
permanent member of the created project you need to change the ownership of the reports to a user that will have main
manager rights in the created project. To do this, log in as the intended main manager and select the menu alternative
"More../Reports and trends/Create and share reports..." in the Schedule, Product backlog and QA views. Then, right-click
on each report in the list and select "Transfer ownership to me" from the popup menu.

Limitations
-----------
The following is not handled as part of cloning a project:
* Documents that are stored in the document section of Hansoft (attachments to tasks are handled though).
* Any workflows/pipelines that are set on tasks or set as default workflows/pipelines will not be handled.
* Committment of product backlog items to the schedule or sprints will not be handled.
* The bug workflow will not be handled.

Terms and conditions
--------------------
HansoftCloneProjectClientPlugin by Svante Lidman (Hansoft AB) is licensed under what is known as an MIT License
as stated in the [LICENSE.md](LICENSE.md).

This program is not part of the official Hansoft product or subject to its license agreement.
The program is provided as is and there is no obligation on Hansoft AB to provide support, update or enhance this program.
Questions can be sent to svante.lidman@hansoft.com and will be answered when other obligations so permit.

Building the program
--------------------
The program can be built with the freely available [Visual Studio Express 2012 for Desktop] [1]. 
You also need the [Hansoft SDK] [2] to be able to build the program. You will also need to
update the references to the  Hansoft SDK in the Visual Studio project.

[1]: http://www.microsoft.com/visualstudio/eng/products/visual-studio-express-for-windows-desktop  "Visual Studio Express 2012 for Desktop"
[2]: http://hansoft.com/support/downloads/                                                         "Hansoft SDK"

Installation
------------
Install the client plugin with the [HansoftClientPluginInstaller] [1].

The plugin should be deployed together with [Jean for Hansoft] [2] and the [CloneProjectBehavior] [3] must be enabled for the Hansoft database
that you want to clone projects in. You activate the CloneProjectBehavior by simply adding a
<CloneProjectBehavior /> to the settings file for Jean.

[1]: http://github.com/Hansoft/HansoftClientPluginInstaller         "HansoftClientPluginInstaller"
[2]: http://github.com/Hansoft/Hansoft-Jean-Jean                    "Jean for Hansoft"
[3]: http://github.com/Hansoft/Hansoft-Jean-CloneProjectBehavior    "CloneProjectBehavior"







































































