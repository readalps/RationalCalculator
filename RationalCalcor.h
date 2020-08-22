#pragma once

#include <stdint.h>
#include <string>
#include <stack>
#include <map>
#include <vector>
#include <sstream>

typedef uint64_t u64;

void trimString(std::string& strTrim);

#define isAddOrMinus(c) ((c) == '+' || (c) == '-')
#define isAsteriskOrSlash(c) ((c) == '*' || (c) == '/')
#define isNumChar(c) ((c) >= '0' && (c) <= '9')

enum EnumError {E_ERR_OK, E_ERR_EMPTY, E_ERR_ZERO_DENOMINATOR, E_ERR_INVALID, E_ERR_PAREN_UNMATCHED};
enum EnumOperItemType {E_OP_ITEM_LPAREN, E_OP_ITEM_NUM, E_OP_ITEM_SIGN};

struct SFraction
{
    u64 numerator;
    u64 denominator;
    bool bNegative;

    SFraction() {
        numerator = 0;
        denominator = 1;
        bNegative = false;
    }

    std::string toStr(bool bFinal = false) const;
    std::string toDecimalStr() const;
};

u64 calcGreatestCommonDivisor(u64 valA, u64 valB);
u64 calcLeastCommonMultiple(u64 valA, u64 valB);
void simplifyFraction(SFraction& oFract);

struct SOperItem
{
    EnumOperItemType eOpItemType;
    char cSign;
    SFraction oFract;

    SOperItem(EnumOperItemType eType) {
        eOpItemType = eType;
    }
    SOperItem(EnumOperItemType eType, char cVal) {
        eOpItemType = eType;
        cSign = cVal;
    }
    SOperItem(EnumOperItemType eType, const SFraction& oVal) {
        eOpItemType = eType;
        oFract = oVal;
    }

    std::string toStr() const
    {
        if (eOpItemType == E_OP_ITEM_NUM) {
            return oFract.toStr();
        }
        else if (eOpItemType == E_OP_ITEM_LPAREN) {
            return "(";
        }
        return std::string() + cSign;
    }
};

enum EnumState {
    E_STATE_EXPECT_NUM = 0, // expecting a rational number (or left paren)
    E_STATE_EXPLICIT,       // with explicit + or - ahead of number (or left paren)
    E_STATE_MEET_NUM,       // part of a number been met
    E_STATE_EXPECT_SIGN,    // expecting one of operation signs (+-*/) or right paren
};

class CRationalCalcor
{
public:
    CRationalCalcor(const std::string& strVal) {
        m_strExpression = strVal;
        m_eState = E_STATE_EXPECT_NUM;
        m_nParenLevel = 0;
        resetNum();
    }
    EnumError calcIt();
    EnumError getResult(SFraction& oVal);

private:
    EnumError dealLeftParen();
    EnumError dealCharWhenExplicit(char ch);
    EnumError dealCharWhenExpectingNum(char ch);
    EnumError dealCharWhenExpectingSign(char ch, size_t idx);
    EnumError dealCharWhenMeetNum(char ch, size_t idx);

    SFraction currentNum2Fraction();
    EnumError try2ReduceStack(char ch, size_t idx);
    EnumError reduceStack(char ch, size_t idx);
    void outputCalcDetail(size_t idx = 0);

    std::string m_strExpression;
    EnumState m_eState;
    int m_nParenLevel;
    std::stack<SOperItem> m_stkOpItem;
    SFraction m_oResult;

    bool isDecimal() {return (m_nDigitSumAftDot != 0 || m_nDigitSumAftRec != 0);}
    void resetNum() {
        m_bHavingVal = false;
        m_bNegative = false;
        m_ullIntPart = 0;
        m_bWithDot = false;
        m_bWithRecur = false;
        m_nDigitSumAftDot = 0;
        m_nDigitSumAftRec = 0;
        m_ullValAftDot = 0;
        m_ullValAftRec = 0;
    }

    bool m_bHavingVal;
    bool m_bNegative;
    u64 m_ullIntPart;
    bool m_bWithDot;
    bool m_bWithRecur;
    int m_nDigitSumAftDot; // sum of digits after dot char(.)
    int m_nDigitSumAftRec; // sum of digits after recurring char(`)
    u64 m_ullValAftDot;
    u64 m_ullValAftRec;
};

class CFraction2Decimal
{
public:
    EnumError setFraction(const std::string& strInput);
private:
    u64 m_ullInt;
    std::string m_strDecimal;
    SFraction m_oFract;
};

