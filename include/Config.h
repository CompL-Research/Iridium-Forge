#pragma once

static int getRegularClosureFlag() { return 1; }
static int getConstructorClosureFlag() { return 2; }
static int getDerivedConstructorClosureFlag() { return 3; }
static int getDerivedMethodClosureFlag() { return 4; }
static int getPrivateMethodClosureFlag() { return 5; }
static int getPropInitNoPrivateClosureFlag() { return 6; }
static int getPropInitDerivedNoPrivateClosureFlag() { return 7; }
static int getPropInitPrivateClosureFlag() { return 8; }
static int getPropInitDerivedPrivateClosureFlag() { return 9; }
static int getPrivateDerivedMethodClosureFlag() { return 10; }
static int getStaticPropInitClosureFlag() { return 11; }
static int getStaticPropInitDerivedClosureFlag() { return 12; }
