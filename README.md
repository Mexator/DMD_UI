# DMD_UI
This project is UI for interaction with hospital database which is part of assignment of course "Data Modelling and Databases" in Innopolis University.

## Build
You can build this project using makefile:
```bash
make
```

## Usage
* After start, apllication is automatically connected to database located at local Innopolis servers.
* Using left expanded list you can see the structure of database, its tables and columns.
* Using topbar you can send specified queries. For the first query there will be 2 dialogue windows, which will ask you  
input first and last names of patient. Other queries does not requires any input.
* After select specified query, in will automatically added into bottom window. You can change the query here.
#### :warning: If query contains multiple subqueries (splited by ';'), it will be works iff this query is predefined and selected from topbar
* Clicking on button 'submit' will execute the query. Result will be shown at right window.
