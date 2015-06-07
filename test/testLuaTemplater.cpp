// Copyright Hugh Perkins 2015 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "test/easycl_gtest_supp.h"

#include "templates/LuaTemplater.h"

using namespace std;

TEST( testLuaTemplater, basicsubstitution1 ) {
    string source = "This is my {{avalue}} template.";

    LuaTemplater mytemplate;
    mytemplate.set( "avalue", 3 );
    string result = mytemplate.render(source);
    cout << result << endl;
    string expectedResult = "This is my 3 template.";
    EXPECT_EQ( expectedResult, result );
    string result2 = mytemplate.render(source);
    return;
    EXPECT_EQ( expectedResult, result2 );
}
TEST( testLuaTemplater, basicsubstitution1b ) {
    string source = "This is my {{avalue}} template";

    LuaTemplater mytemplate;
    mytemplate.set( "avalue", 3 );
    string result = mytemplate.render(source);
    cout << result << endl;
    string expectedResult = "This is my 3 template";
    EXPECT_EQ( expectedResult, result );
    string result2 = mytemplate.render(source);
    return;
    EXPECT_EQ( expectedResult, result2 );
}
TEST( testLuaTemplater, basicsubstitution ) {
    string source = "\n"
"        This is my {{avalue}} template.  It's {{secondvalue}}...\n"
"        Today's weather is {{weather}}.\n"
"    \n";

    LuaTemplater mytemplate;
    mytemplate.set( "avalue", 3 );
    mytemplate.set( "secondvalue", 123 );
    mytemplate.set( "weather", "rain" );
    string result = mytemplate.render(source);
    cout << result << endl;
    string expectedResult = "\n"
"        This is my 3 template.  It's 123...\n"
"        Today's weather is rain.\n"
"    \n";
    EXPECT_EQ( expectedResult, result );
    string result2 = mytemplate.render(source);
    return;
    EXPECT_EQ( expectedResult, result2 );
}
//TEST( testLuaTemplater, basicsubstitution_from_vec ) {
//    string source = "\n"
//"        This is my {{avalue}} template.  It's {{secondvalue}}...\n"
//"        Today's weather is {{weather}}.\n"
//"    \n";

//    LuaTemplater mytemplate( source );
//    map< string, Value * > valueByName;
//    valueByName.setValue( "avalue", 3 );
//    valueByName.setValue( "secondvalue", 12.123f );
//    valueByName.setValue( "weather", "rain" );
//    string result = mytemplate.render();
//    cout << result << endl;
//    string expectedResult = "\n"
//"        This is my 3 template.  It's 12.123...\n"
//"        Today's weather is rain.\n"
//"    \n";
//    EXPECT_EQ( expectedResult, result );
//}
//TEST( testLuaTemplater, namemissing ) {
//    string source = "\n"
//"        This is my {{avalue}} template.\n"
//"    \n";

//    LuaTemplater mytemplate;
//    bool threw = false;
//    try {
//        string result = mytemplate.render(source);
//    } catch( render_error &e ) {
//        EXPECT_EQ( string("name avalue not defined"), e.what() );
//        threw = true;
//    }
//    EXPECT_EQ( true, threw );
//}
TEST( testLuaTemplater, startofsection ) {
    string source = "{{avalue}} template";

    LuaTemplater mytemplate;
    mytemplate.set( "avalue", 3 );
    string result = mytemplate.render(source);
    cout << result << endl;
    string expectedResult = "3 template";
    EXPECT_EQ( expectedResult, result );
}
TEST( testLuaTemplater, endofsection ) {
    string source = "template {{avalue}}";

    LuaTemplater mytemplate;
    mytemplate.set( "avalue", 3 );
    string result = mytemplate.render(source);
    cout << result << endl;
    string expectedResult = "template 3";
    EXPECT_EQ( expectedResult, result );
}
TEST( testLuaTemplater, loop ) {
    string source = "\n"
"        {% for i=0,its-1 do %}\n"
"            a[{{i}}] = image[{{i}}];\n"
"        {% end %}\n"
"    \n";

    LuaTemplater mytemplate;
    mytemplate.set( "its", 3 );
    string result = mytemplate.render(source);
    cout << result << endl;
    string expectedResult = "\n"
"        \n"
"            a[0] = image[0];\n"
"        \n"
"            a[1] = image[1];\n"
"        \n"
"            a[2] = image[2];\n"
"        \n"
"    \n";
    EXPECT_EQ( expectedResult, result );
}

TEST( testLuaTemplater, nestedloop ) {
    string source = "\n"
"{% for i=0,its-1 do %}a[{{i}}] = image[{{i}}];\n"
"{% for j=0,1 do %}b[{{j}}] = image[{{j}}];\n"
"{% end %}{% end %}\n"
"";

    LuaTemplater mytemplate;
    mytemplate.set( "its", 3 );
    string result = mytemplate.render(source);
    cout << "[" << result << "]" << endl;
    string expectedResult = "\n"
"a[0] = image[0];\n"
"b[0] = image[0];\n"
"b[1] = image[1];\n"
"a[1] = image[1];\n"
"b[0] = image[0];\n"
"b[1] = image[1];\n"
"a[2] = image[2];\n"
"b[0] = image[0];\n"
"b[1] = image[1];\n"
"\n"
"";
    EXPECT_EQ( expectedResult, result );
}

TEST( testLuaTemplater, foreachloop ) {
    string source = "\n"
        "{% for _,name in ipairs(names) do %}{{name}}\n"
        "{% end %}\n"
        "";
    LuaTemplater mytemplate;
//    cout << mytemplate << endl;
    vector<string> values;
    values.push_back("blue");
    values.push_back("green");
    values.push_back("red");
    mytemplate.set( "names", values );
    string result = mytemplate.render(source);
    cout << "[" << result << "]" << endl;
    string expectedResult = "\n"
"blue\n"
"green\n"
"red\n"
"\n";
    EXPECT_EQ( expectedResult, result );
}

