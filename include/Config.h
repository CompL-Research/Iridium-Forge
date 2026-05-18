#pragma once


static constexpr int CF_TOP_LEVEL_MODULE = 100;
static constexpr int CF_TOP_LEVEL_SCRIPT = 101;
static constexpr int CF_ARROW_FUNCTION   = 102;
static constexpr int CF_FUNCTION         = 103;
static constexpr int CF_CTR              = 104;
static constexpr int CF_DERIVED_CTR      = 105;
static constexpr int CF_CLASS_METHOD     = 106;
static constexpr int CF_PROP_INIT        = 107;


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
