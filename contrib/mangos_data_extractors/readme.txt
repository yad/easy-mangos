This readme describes how to extract from the client the dbcs and maps (and optionally vmaps) which are needed to run MaNGOS.

Step 0)
Windows:
    Open the sln file in contrib/mangos_data_extractor/win/ for your version of VC
    Change to Release configuration
    Build All


Step 1)
Copy the contents of contrib/mangos_data_extractor/ to your WoW directory
You do NOT need the win folder or any of its contents.


Step 2)
Pick ONE of the following and run it FROM THE WoW DIRECTORY.  Running both is a WASTE OF TIME.

data.bat:
    extracts DBC and Map data
    Use this if you AREN'T going to enable VMaps

    After the process is complete, you should have two (2) new directories:
        dbc
        maps

data+vmap.bat:
    extracts DBC, Map, and Model data, and converts Map and Model data into VMaps
    Use this if you ARE going to enable VMaps

    After the process is complete, you should have four new directories:
        Buildings
        dbc
        maps
        vmaps
    Buildings is an intermediate directory for building vmaps, so you may delete it if you wish.


Step 3)
Copy the generated directories to your mangos directory