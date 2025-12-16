PRAGMA foreign_keys = 0;

DROP TABLE IF EXISTS "Stockage";
CREATE TABLE IF NOT EXISTS "Stockage" (
	"ID"	INTEGER PRIMARY KEY AUTOINCREMENT,
        "Location"	TEXT NOT NULL
);

DROP TABLE IF EXISTS "Manufacturer";
CREATE TABLE IF NOT EXISTS "Manufacturer" (
	"Manuf_ID"	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
	"Name"	TEXT NOT NULL,
	"Site_Web"	TEXT
);
DROP TABLE IF EXISTS "Board";
CREATE TABLE IF NOT EXISTS "Board" (
	"Board_ID"	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
	"Board_Name"	TEXT,
	"Board_serial"	TEXT,
	"Model"	TEXT
);
insert into Board (Board) VALUES (0,'Trash','Trash','Trash');

DROP TABLE IF EXISTS "Status";
CREATE TABLE IF NOT EXISTS "Status" (
	"ID"	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
	"Status"	TEXT NOT NULL
);
insert into Status (Status) VALUES ("Ordered");
insert into Status (Status) VALUES ("Received");
DROP TABLE IF EXISTS "Procurement Company";
CREATE TABLE IF NOT EXISTS "Procurement Company" (
	"Proc_ID"	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
	"Name"	TEXT NOT NULL,
	"Site_web"	TEXT
);

DROP TABLE IF EXISTS "Proc. Company Contact";
DROP TABLE IF EXISTS "Procurement Company Contact";
CREATE TABLE IF NOT EXISTS "Procurement Company Contact" (
	"Contact_ID"	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
	"Name"	TEXT NOT NULL,
	"First_name"	TEXT NOT NULL,
	"Phone_Number"	TEXT,
	"Email"	TEXT,
	"Proc_company"	INTEGER NOT NULL,
	FOREIGN KEY("Proc_company") REFERENCES "Procurement Company"("Proc_ID")
);

DROP TABLE IF EXISTS "Composants";
CREATE TABLE IF NOT EXISTS "Composants" (
	"ID"	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
	"Family"	TEXT,
	"Group_code"	TEXT,
	"Description"	TEXT,
	"Part_Number"	TEXT NOT NULL,
	"Value"	TEXT,
	"Tolerance"	TEXT,
	"Maximum_rating"	TEXT,
	"Package"	TEXT,
	"Manuf_ID"	INTEGER,
	"Generic_specification"	TEXT,
	"Detailed_specification"	TEXT,
	"Quality_level"	TEXT,
	"Space_Qualified"	BOOLEAN,
	"Procurement_Company"	INTEGER,
	"ITAR_EAS"	BOOLEAN,
	"Date_Code"	TEXT,
	"Relifed"	BOOLEAN,
	"Weight"	DOUBLE,
	"Status_ID"	INTEGER NOT NULL,
	"Date_reception"	DATETIME,
	"Quantity_ordered"	INTEGER,
	"Stock_location"	INTEGER,
	FOREIGN KEY("Stock_location") REFERENCES "Stockage"("ID"),
	FOREIGN KEY("Manuf_ID") REFERENCES "Manufacturer"("Manuf_ID"),
	FOREIGN KEY("Procurement_Company") REFERENCES "Procurement Company"("Proc_ID"),
	FOREIGN KEY("Status_ID") REFERENCES "Status"("ID")
);
DROP TABLE IF EXISTS "DCL";
CREATE TABLE IF NOT EXISTS "DCL" (
	"ID"	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
	"Composant_ID"	INTEGER NOT NULL,
	"Board_ID"	INTEGER NOT NULL,
	"Ref_Topo"	TEXT NOT NULL,
	"Serial_Number"	TEXT,
	FOREIGN KEY("Board_ID") REFERENCES "Board"("Board_ID"),
	FOREIGN KEY("Composant_ID") REFERENCES "Composants"("ID")
);
PRAGMA foreign_keys = 1;
