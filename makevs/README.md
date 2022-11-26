In the October 2022 versions of j, to compile jdll with libgmp under visual studio, you need to set the IMPORTGMPLIB environmental variable.

CAUTION: the following seems like it should work, but in fact turned out to be useless.

This could be set outside of visual studio as a system or user environmental variable. However, it should be possible to set it within visual studio as an environmental variable for the jdll project.

However, while this approach in Visual Studio 2019 community edition does NOT set the environmental variable:

(1) Select the project in solution explorer

(2) Bring up property pages for the project (shift-F4 or View > Property Pages or in solution explorer with the project selected: alt-enter or right click Properties)

(3) In that popup dialog, under Configuration Properties > Debugging > Environment on the far right, click the "drop down arrow" and select "Edit...".

In the edittable region which appears add a line which says:

IMPORTGMPLIB=1

(4) Select OK to close the environmental variable edit mechanism and select OK again to close the Configuration Properties popup.

----------------------------------------------------------------
