/*
 * Copyright (c) 2012 Dan Wilcox <danomatika@gmail.com>
 *
 * BSD Simplified License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * See https://github.com/danomatika/ofxLua for documentation
 *
 */
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetVerticalSync(true);
	ofSetFrameRate(30);
	ofSetLogLevel("ofxLua", OF_LOG_VERBOSE);
		
	// scripts to run
	scripts.push_back("scripts/graphicsExample.lua");
	scripts.push_back("scripts/imageLoaderExample.lua");
	scripts.push_back("scripts/polygonExample.lua");
	scripts.push_back("scripts/fontsExample.lua");
	scripts.push_back("scripts/boringTests.lua");
	currentScript = 0;
	
	// init the lua state
	lua.init();
	
	// listen to error events
	lua.addListener(this);
	
	// run some read/write api tests
	runTests();
	
	// reinit the lua state, clears test data in state
	lua.init(true); // true because we want to stop on an error
	
	// run a script
	// true = change working directory to the script's parent dir
	// so lua will find scripts with relative paths via require
	// note: changing dir does *not* affect the OF data path
	lua.doScript(scripts[currentScript], true);
	
	// call the script's setup() function
	lua.scriptSetup();
}

//--------------------------------------------------------------
void ofApp::update() {
	// call the script's update() function
	lua.scriptUpdate();
}

//--------------------------------------------------------------
void ofApp::draw() {
	// call the script's draw() function
	lua.scriptDraw();
	
	ofSetColor(0);
	ofDrawBitmapString("use <- & -> to change between scripts", 10, ofGetHeight()-22);
	ofDrawBitmapString(scripts[currentScript], 10, ofGetHeight()-10);
}

//--------------------------------------------------------------
void ofApp::exit() {
	// call the script's exit() function
	lua.scriptExit();
	
	// clear the lua state
	lua.clear();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	
	switch(key) {
	
		case 'r':
			reloadScript();
			break;
	
		case OF_KEY_LEFT:
			prevScript();
			break;
			
		case OF_KEY_RIGHT:
			nextScript();
			break;
			
		case ' ':
			lua.doString("print(\"this is a lua string saying you hit the space bar!\")");
			break;
	}
	
	lua.scriptKeyPressed(key);
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	lua.scriptMouseMoved(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	lua.scriptMouseDragged(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	lua.scriptMousePressed(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	lua.scriptMouseReleased(x, y, button);
}

//--------------------------------------------------------------
void ofApp::errorReceived(string& msg) {
	ofLogNotice() << "got a script error: " << msg;
}

//--------------------------------------------------------------
void ofApp::reloadScript() {
	// exit, reinit the lua state, and reload the current script
	lua.scriptExit();
	lua.init();
	lua.doScript(scripts[currentScript], true);
	lua.scriptSetup();
}

void ofApp::nextScript() {
	currentScript++;
	if(currentScript > scripts.size()-1)
		currentScript = 0;
	reloadScript();
}

void ofApp::prevScript() {
	currentScript--;
	if(currentScript < 0)
		currentScript = scripts.size()-1;
	reloadScript();
}

//--------------------------------------------------------------
 void ofApp::runTests() {

 	// do tests
 	//------
 	ofLog();
 	ofLog() << "*** BEGIN READ TEST ***";
	
 	// load a script with some variables we want
 	lua.doScript("variableTest.lua");

	// prints global table if no table is pushed
	//lua.printTable();

 	// print the variables in the script manually
 	ofLog() << "variableTest variables:";
 	ofLog() << "	abool: " << lua.getBool("abool");
 	ofLog() << "	anumber: " << lua.getNumber("anumber");
 	ofLog() << "	astring: " << lua.getString("astring");
	
 	// load simple table arrays by type
 	stringstream line;
	
 	vector<bool> boolTable;
 	lua.getBoolVector("boolTable", boolTable);
 	line << "	boolTable: ";
 	for(int i = 0; i < boolTable.size(); ++i) {
 		line << boolTable[i] << " ";
	}
 	ofLog() << line.str() << "#: " << lua.tableSize("boolTable");
 	line.str(""); // clear
	
 	vector<lua_Number> numberTable;
 	lua.getNumberVector("numberTable", numberTable);
 	line << "	numberTable: ";
 	for(int i = 0; i < numberTable.size(); ++i) {
 		line << numberTable[i] << " ";
	}
 	ofLog() << line.str() << "#: " << lua.tableSize("numberTable");
 	line.str(""); // clear
	
 	vector<string> stringTable;
 	lua.getStringVector("stringTable", stringTable);
 	line << "	stringTable: ";
 	for(int i = 0; i < stringTable.size(); ++i) {
 		line << "\"" << stringTable[i] << "\" ";
	}
 	ofLog() << line.str() << "#: " << lua.tableSize("stringTable");
 	line.str(""); // clear
	
 	// try to load a mixed var table, should fail and issue warnings
	ofLog() << " ### should be warnings here vvv";
	vector<string> testStringVector;
 	lua.getStringVector("mixedTable", testStringVector);
	ofLog() << " ### should be warnings here ^^^";
	
 	// read manually by index, lua indices start at 1 not 0!
 	lua.pushTable("mixedTable");
 	ofLog() << "mixedTable";
 	for(int i = 1; i <= lua.tableSize(); ++i) {
 		if(lua.isBool(i)) {
 			ofLogNotice() << "\t" << i << " b: " << lua.getBool(i);
 		}
 		else if(lua.isNumber(i)) {
 			ofLogNotice() << "\t" << i << " n: " << lua.getNumber(i);
 		}
 		else if(lua.isString(i)) {
 			ofLogNotice() << "\t" << i << " s: " << lua.getString(i);
 		}
 	}
 	lua.popTable();
    
	// load a table within a table by name
	lua.pushTable("atable");
	lua.getStringVector("stringTable", stringTable);
	line << "atable.stringTable: ";
 	for(int i = 0; i < stringTable.size(); ++i) {
 		line << "\"" << stringTable[i] << "\" ";
	}
 	ofLog() << line.str() << "#: " << lua.tableSize("stringTable");
 	line.str(""); // clear
	lua.popTable();
	 
	 // load a table within a table by index
	lua.pushTable("atable");
	lua.pushTable("nestedTable");
	lua.getNumberVector(2, numberTable);
	line << "atable.nestedTable[2]: ";
 	for(int i = 0; i < numberTable.size(); ++i) {
 		line << numberTable[i] << " ";
	}
 	ofLog() << line.str() << "#: " << lua.tableSize(2);
 	line.str(""); // clear
	lua.popAllTables();
	
 	// print the contents of the "atable" table
 	lua.pushTable("atable"); // move from the global lua namespace to the "atable" table
 	lua.printTable(); // print variables & tables in "atable"
 	lua.popTable(); // return to the global namespace
	
	// check if testing existence within a table works
	lua.pushTable("atable");
	ofLog() << "atable.afunction a function?: " << lua.isFunction("afunction");
	lua.pushTable("nestedTable");
	lua.pushTable(1);
	ofLog() << "atable.nestedTable[1][1] a number?: " << lua.isNumber(1);
	lua.popAllTables();
	
 	ofLog() << "*** END READ TEST ***" << endl;
	
 	//------

 	ofLog() << "*** BEGIN WRITE TEST ***";

 	// print
 	ofLog() << "values before:";
 	ofLog() << "	abool: " << lua.getBool("abool");
 	ofLog() << "	anumber: " << lua.getNumber("anumber");
 	ofLog() << "	astring: " << lua.getString("astring");

 	// this should throw a warning, it dosen't exist yet
	ofLog() << "### should be a warning here vvv";
 	ofLog() << "	newstring: " << lua.getString("newstring");
	ofLog() << "### should be a warning here ^^^";

 	numberTable.clear();
 	lua.getNumberVector("numberTable", numberTable);
 	line << "	numberTable: ";
 	for(int i = 0; i < numberTable.size(); ++i) {
 		line << numberTable[i] << " ";
	}
 	ofLog() << line.str() << "#: " << lua.tableSize("numberTable");
 	line.str(""); // clear
	
 	// set values
 	lua.setBool("abool", false);
 	lua.setNumber("anumber", 66.6);
 	lua.setString("astring", "kaaaaa");
	
 	// add new value
 	lua.setString("newstring", "a new string");
	
 	// set vector
 	numberTable.clear();
 	for(int i = 0; i < 10; i+=2) {
 		numberTable.push_back(i);
 	}
 	lua.setNumberVector("numberTable", numberTable);
	
 	// print again
 	ofLog() << "values after:";
 	ofLog() << "	abool: " << lua.getBool("abool");
 	ofLog() << "	anumber: " << lua.getNumber("anumber");
 	ofLog() << "	astring: " << lua.getString("astring");
 	ofLog() << "	newstring: " << lua.getString("newstring");
	
 	numberTable.clear();
 	lua.getNumberVector("numberTable", numberTable);
 	line << "	numberTable: ";
 	for(int i = 0; i < numberTable.size(); ++i)
 		line << numberTable[i] << " ";
 	ofLog() << line.str() << "#: " << lua.tableSize("numberTable");
 	line.str(""); // clear
	
 	// write manually by index, remember lua indices start at 1 not 0!
 	lua.pushTable("mixedTable");
 	for(int i = 1; i <= lua.tableSize(); ++i) {
 		if(lua.isBool(i)) {
 			lua.setBool(i, true);
 		}
 		else if(lua.isNumber(i)) {
 			lua.setNumber(i, 9999.99);
 		}
 		else if(lua.isString(i)) {
 			lua.setString(i, "abcdefg");
 		}
 	}
 	lua.printTable();
 	lua.popTable();

 	ofLog() << "*** END WRITE TEST ***" << endl;

 	//------
	
 	ofLog() << "*** BEGIN EXIST TEST ***";
	
 	// "avar" dosen't exist
 	ofLog() << "avar exists: " << lua.isNumber("avar")
 		<< ", is nil: " << lua.isNil("avar");
	
 	// "avar" exists and is equal to 99
 	lua.setNumber("avar", 99);
 	ofLog() << "avar exists: " << lua.isNumber("avar")
 		<< ", is nil: " << lua.isNil("avar");
 	ofLog() << "	avar: " << lua.getNumber("avar");
	
 	// set "avar" to nil, it no longer exists
 	lua.setNil("avar");
 	ofLog() << "avar exists: " << lua.isNumber("avar")
 		<< ", is nil: " << lua.isNil("avar");
	
 	ofLog() << "*** END EXIST TEST ***" << endl;
	
 	//------
	
 	ofLog() << "*** BEGIN CLEAR TEST ***";
	
 	lua.printTable("anotherTable");
 	lua.clearTable("anotherTable");
	ofLog() << "### should only print the table name vvv";
 	lua.printTable("anotherTable"); // should only print the name
	
 	ofLog() << "*** END CLEAR TEST ***" << endl;
	
 	//------
	
 	ofLog() << "*** BEGIN FILE WRITER TEST ***";
	
 	// write text & vars out into a text file
 	ofxLuaFileWriter luaWriter;
 	string filename = "writerTest.lua";
 	luaWriter.writeComment("lua writer test");
 	luaWriter.newLine();
 	luaWriter.beginCommentBlock();
 		luaWriter.writeLine("this is a comment block");
 	luaWriter.endCommentBlock();
 	luaWriter.newLine();
 	luaWriter.writeBool("abool", lua.getBool("abool"));
 	luaWriter.writeNumber("anumber", lua.getNumber("anumber"));
 	luaWriter.writeString("astring", lua.getString("astring"));
	luaWriter.beginTable("vectors");
		luaWriter.writeBoolVector("boolTable", boolTable);
		luaWriter.writeNumberVector("numberTable", numberTable);
		luaWriter.writeStringVector("stringTable", stringTable);
	luaWriter.endTable();
	
 	// write a table's contents recursively into the file
 	lua.writeTable("atable", luaWriter, true);
	
 	// save, load, and print file
 	if(luaWriter.saveToFile(filename)) {
		
 		// print
		ofLog() << "### Written File vvv";
 		ofBuffer b = ofBufferFromFile(filename);
 		for(auto &line : b.getLines()) {
 			ofLog() << line;
 		}
 		b.clear();
		ofLog() << "### Written File ^^^";
		
 		// try loading into lua state
 		lua.doScript(filename);
		
 		// delete when done
 		ofFile::removeFile(filename);
 	}
	
 	ofLog() << "*** END FILE WRITER TEST ***" << endl;
	
 	//-------
	
	ofLog() << "*** CHECK STACK ***";
	ofLog() << "Tests Done, stack length should be 0";
	lua.printStack();
	ofLog() << "*** TESTS DONE ***" << endl;
 }
