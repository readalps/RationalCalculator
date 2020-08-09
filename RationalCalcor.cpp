#include "RationalCalcor.h"

void trimString(std::string& strTrim)
{
    int iHeadPos = strTrim.find_first_not_of(" \t");
    if (iHeadPos < 0)
    {
        strTrim = "";
        return;
    }
    int iTailPos = strTrim.find_last_not_of(" \t");
    if (strTrim.length() - 1 != iTailPos)
    {
        strTrim.erase(iTailPos + 1);
    }
    if (0 != iHeadPos)
    {
        strTrim.erase(0, iHeadPos);
    }
}

SFraction minusByZero(const SFraction& oVal)
{
    SFraction oRet = oVal;
    oRet.bNegative = (!oRet.bNegative);
    return oRet;
}

SFraction fractAdd(const SFraction& oL, const SFraction& oR)
{
    SFraction oRslt;
    /// having same denominator
    if (oL.denominator == oR.denominator)
    {
        oRslt.denominator = oL.denominator;
        if (oL.bNegative == oR.bNegative)
        {
            oRslt.numerator = (oL.numerator + oR.numerator);
            oRslt.bNegative = oL.bNegative;
            simplifyFraction(oRslt);
            return oRslt;
        }
        bool bCmp = oL.numerator >= oR.numerator;
        oRslt.numerator = (bCmp ? oL.numerator - oR.numerator : oR.numerator - oL.numerator);
        oRslt.bNegative = (bCmp ? oL.bNegative : oR.bNegative);
        simplifyFraction(oRslt);
        return oRslt;
    }
    /// having different denominator
    u64 lcm = calcLestCommonMultiple(oL.denominator, oR.denominator);
    oRslt.denominator = lcm;
    u64 numL = oL.numerator * (lcm / oL.denominator);
    u64 numR = oR.numerator * (lcm / oR.denominator);
    if (oL.bNegative == oR.bNegative)
    {
        oRslt.numerator = numL + numR;
        oRslt.bNegative = oL.bNegative;
        simplifyFraction(oRslt);
        return oRslt;
    }
    bool bCmp = (numL >= numR);
    oRslt.numerator = (bCmp ? numL - numR : numR - numL);
    oRslt.bNegative = (bCmp ? oL.bNegative : oR.bNegative);
    simplifyFraction(oRslt);
    return oRslt;
}

SFraction reciprocal(const SFraction& oVal)
{
    SFraction oRet;
    oRet.numerator = oVal.denominator;
    oRet.denominator = oVal.numerator;
    oRet.bNegative = oVal.bNegative;
    return oRet;
}

SFraction fractMultiply(const SFraction& oL, const SFraction& oR)
{
    SFraction oRslt;
    oRslt.numerator = oL.numerator * oR.numerator;
    oRslt.denominator = oL.denominator * oR.denominator;
    oRslt.bNegative = (oL.bNegative != oR.bNegative);
    simplifyFraction(oRslt);
    return oRslt;
}

u64 calcGcdInn(u64 valBig, u64 valSmall)
{
    u64 remainder = valBig % valSmall;
    if (remainder == 0)
    {
        return valSmall;
    }
    return calcGcdInn(valSmall, remainder);
}

u64 calcGreatestCommonDivisor(u64 valA, u64 valB)
{
    if (valA == valB)
    {
        return valA;
    }
    return (valA > valB ? calcGcdInn(valA, valB) : calcGcdInn(valB, valA));
}

u64 calcLestCommonMultiple(u64 valA, u64 valB)
{
    return valA / calcGreatestCommonDivisor(valA, valB) * valB;
}

void simplifyFraction(SFraction& oFract)
{
    if (oFract.denominator == 1)
    {
        return;
    }
    if (oFract.numerator == 0)
    {
        oFract.denominator = 1;
        return;
    }
    u64 gcd = calcGreatestCommonDivisor(oFract.numerator, oFract.denominator);
    if (gcd != 1)
    {
        oFract.numerator /= gcd;
        oFract.denominator /= gcd;
    }
}

EnumError doOper(const SFraction& oL, const SFraction& oR, char cSign, SFraction& oResult)
{
    if (cSign == '+')
    {
        oResult = fractAdd(oL, oR);
    }
    else if (cSign == '-')
    {
        oResult = fractAdd(oL, minusByZero(oR));
    }
    else if (cSign == '*')
    {
        oResult = fractMultiply(oL, oR);
    }
    else if (cSign == '/')
    {
        if (oR.numerator == 0)
        {
            return E_ERR_ZERO_DENOMINATOR;
        }
        oResult = fractMultiply(oL, reciprocal(oR));
    }
    else
    {
        return E_ERR_INVALID;
    }
    return E_ERR_OK;
}

EnumError CRationalCalcor::getResult(SFraction& oVal)
{
    if (m_bHavingVal)
    {
        SFraction oFract = currentNum2Fraction();
        SOperItem oItem(E_OP_ITEM_NUM, oFract);
        m_stkOpItem.push(oItem);
        if (m_stkOpItem.size() != 1 && isDecimal())
        {
            outputCalcDetail();
        }
        resetNum();
    }
    while (m_stkOpItem.size() >= 3)
    {
        SFraction oR = m_stkOpItem.top().oFract;
        m_stkOpItem.pop();
        char ch = m_stkOpItem.top().cSign;
        m_stkOpItem.pop();
        SFraction oL = m_stkOpItem.top().oFract;
        m_stkOpItem.pop();
        SFraction oVal;
        EnumError eErr = doOper(oL, oR, ch, oVal);
        if (eErr != E_ERR_OK)
        {
            return eErr;
        }
        SOperItem oItem(E_OP_ITEM_NUM, oVal);
        m_stkOpItem.push(oItem);
        if (m_stkOpItem.size() != 1)
        {
            outputCalcDetail();
        }
    }
    if (m_stkOpItem.empty() || m_stkOpItem.size() == 2)
    {
        return E_ERR_INVALID;
    }
    oVal = m_stkOpItem.top().oFract;
    simplifyFraction(oVal);
    return E_ERR_OK;
}

EnumError CRationalCalcor::dealLeftParen()
{
    if (m_eState != E_STATE_EXPECT_NUM && m_eState != E_STATE_EXPLICIT)
    {
        return E_ERR_INVALID;
    }
    if (m_eState == E_STATE_EXPLICIT && m_bNegative)
    {
        m_bNegative = false;
        SFraction oZero;
        SOperItem oItemZero(E_OP_ITEM_NUM, oZero);
        m_stkOpItem.push(oItemZero);
        SOperItem oItemMinus(E_OP_ITEM_SIGN, '-');
        m_stkOpItem.push(oItemMinus);
    }
    m_eState = E_STATE_EXPECT_NUM;
    SOperItem oItem(E_OP_ITEM_LPAREN);
    m_stkOpItem.push(oItem);
    ++m_nParenLevel;
    return E_ERR_OK;
}

EnumError CRationalCalcor::dealCharWhenExplicit(char ch)
{
    if (!isNumChar(ch) && ch != '.')
    {
        return E_ERR_INVALID;
    }
    if (ch == '.')
    {
        m_bWithDot = true;
        m_bHavingVal = true;
        m_eState = E_STATE_MEET_NUM;
        return E_ERR_OK;
    }
    m_ullIntPart = m_ullIntPart * 10 + (u64)(ch - '0');
    m_bHavingVal = true;
    m_eState = E_STATE_MEET_NUM;
    return E_ERR_OK;
}

EnumError CRationalCalcor::dealCharWhenExpectingSign(char ch, size_t idx)
{
    if (ch != ')' && !isAddOrMinus(ch) && !isAsteriskOrSlash(ch))
    {
        return E_ERR_INVALID;
    }
    EnumError eErr = try2ReduceStack(ch, idx);
    if (eErr != E_ERR_OK)
    {
        return eErr;
    }
    return E_ERR_OK;
}

u64 powerBase10(int num)
{
    u64 ret = 1;
    for (int idx = 0; idx < num; ++idx)
    {
        ret = ret * 10;
    }
    return ret;
}

SFraction CRationalCalcor::currentNum2Fraction()
{
    SFraction oFract;
    oFract.numerator = m_ullIntPart;
    if (m_bWithDot && m_nDigitSumAftDot)
    {
        SFraction oDec;
        oDec.numerator = m_ullValAftDot;
        oDec.denominator = powerBase10(m_nDigitSumAftDot);
        oFract = fractAdd(oFract, oDec);
    }
    if (m_bWithRecur && m_nDigitSumAftRec)
    {
        SFraction oRec;
        oRec.numerator = m_ullValAftRec;
        oRec.denominator = (powerBase10(m_nDigitSumAftRec) - 1) * powerBase10(m_nDigitSumAftDot);
        oFract = fractAdd(oFract, oRec);
    }
    oFract.bNegative = m_bNegative;
    return oFract;
}

EnumError CRationalCalcor::dealCharWhenMeetNum(char ch, size_t idx)
{
    if (ch == ',')
        return E_ERR_OK;
    if (ch == '.')
    {
        if (m_bWithDot)
            return E_ERR_INVALID;
        m_bWithDot = true;
        return E_ERR_OK;
    }
    if (ch == '`')
    {
        if (!m_bWithDot || m_bWithRecur)
            return E_ERR_INVALID;
        m_bWithRecur = true;
        return E_ERR_OK;
    }
    if (isNumChar(ch))
    {
        if (!m_bWithDot)
        {
            m_ullIntPart = m_ullIntPart * 10 + (u64)(ch - '0');
            return E_ERR_OK;
        }
        if (!m_bWithRecur)
        {
            m_ullValAftDot = m_ullValAftDot * 10 + (u64)(ch - '0');
            ++m_nDigitSumAftDot;
            return E_ERR_OK;
        }
        m_ullValAftRec = m_ullValAftRec * 10 + (u64)(ch - '0');
        ++m_nDigitSumAftRec;
        return E_ERR_OK;
    }
    /// ch does not belong to the current rational number
    SFraction oFract = currentNum2Fraction();
    SOperItem oItem(E_OP_ITEM_NUM, oFract);
    m_stkOpItem.push(oItem);
    if (isDecimal())
    {
        outputCalcDetail(idx);
    }
    resetNum();
    if (ch == ')' || isAddOrMinus(ch) || isAsteriskOrSlash(ch))
    {
        EnumError eErr = try2ReduceStack(ch, idx);
        if (eErr != E_ERR_OK)
        {
            return eErr;
        }
    }
    else
    {
        return E_ERR_INVALID;
    }
    return E_ERR_OK;
}

EnumError CRationalCalcor::dealCharWhenExpectingNum(char ch)
{
    if (!isAddOrMinus(ch) && !isNumChar(ch) && ch != '.')
    {
        return E_ERR_INVALID;
    }
    if (isAddOrMinus(ch))
    {
        m_bNegative = (ch == '-');
        m_eState = E_STATE_EXPLICIT;
        return E_ERR_OK;
    }
    if (ch == '.')
    {
        m_bWithDot = true;
        m_bHavingVal = true;
        m_eState = E_STATE_MEET_NUM;
        return E_ERR_OK;
    }
    m_ullIntPart = (u64)(ch - '0');
    m_bHavingVal = true;
    m_eState = E_STATE_MEET_NUM;
    return E_ERR_OK;
}

EnumError CRationalCalcor::calcIt()
{
    trimString(m_strFormula);
    if (m_strFormula.empty())
        return E_ERR_EMPTY;
    resetNum();
    EnumError eRet = E_ERR_OK;
    for (size_t idx = 0; idx < m_strFormula.length(); ++idx)
    {
        char ch = m_strFormula[idx];
        if (ch == ' ' || ch == '\t')
            continue;
        if (ch == '(')
        {
            if ((eRet = dealLeftParen()) != E_ERR_OK)
                return eRet;
            continue;
        }
        switch (m_eState)
        {
        case E_STATE_EXPECT_SIGN:
            if ((eRet = dealCharWhenExpectingSign(ch, idx)) != E_ERR_OK)
                return eRet;
        	break;
        case E_STATE_EXPECT_NUM:
            if ((eRet = dealCharWhenExpectingNum(ch)) != E_ERR_OK)
                return eRet;
            break;
        case E_STATE_EXPLICIT:
            if ((eRet = dealCharWhenExplicit(ch)) != E_ERR_OK)
                return eRet;
            break;
        case E_STATE_MEET_NUM:
            if ((eRet = dealCharWhenMeetNum(ch, idx)) != E_ERR_OK)
                return eRet;
            break;
        default:
            return E_ERR_INVALID;
        }
    } // end of loop
    return E_ERR_OK;
}

EnumError CRationalCalcor::try2ReduceStack(char ch, size_t idx)
{
    if (m_stkOpItem.empty())
    {
        return E_ERR_INVALID;
    }
    if (m_stkOpItem.size() == 1)
    {
        if (ch == ')')
        {
            return E_ERR_PAREN_UNMATCHED;
        }
        SOperItem oItem(E_OP_ITEM_SIGN, ch);
        m_stkOpItem.push(oItem);
        m_eState = E_STATE_EXPECT_NUM;
        return E_ERR_OK;
    }
    if (m_stkOpItem.size() == 2)
    {
        if (ch == ')' && m_nParenLevel == 0)
        {
            return E_ERR_PAREN_UNMATCHED;
        }
        return reduceStack(ch, idx);
    }
    if (isAsteriskOrSlash(ch))
    {
        SOperItem oItemNumLast = m_stkOpItem.top();
        m_stkOpItem.pop();
        SOperItem oItemSign = m_stkOpItem.top();
        m_stkOpItem.push(oItemNumLast);
        if (isAddOrMinus(oItemSign.cSign))
        {
            SOperItem oItem(E_OP_ITEM_SIGN, ch);
            m_stkOpItem.push(oItem);
            m_eState = E_STATE_EXPECT_NUM;
            return E_ERR_OK;
        }
    }
    return reduceStack(ch, idx);
}

EnumError CRationalCalcor::reduceStack(char ch, size_t idx)
{
    SOperItem oItemNum2nd = m_stkOpItem.top();
    m_stkOpItem.pop();
    SOperItem oItemSign = m_stkOpItem.top();
    m_stkOpItem.pop();
    if (oItemSign.eOpItemType == E_OP_ITEM_LPAREN)
    {
        if (ch == ')')
        {
            m_stkOpItem.push(oItemNum2nd);
            m_nParenLevel--;
            m_eState = E_STATE_EXPECT_SIGN;
        }
        else
        {
            m_stkOpItem.push(oItemSign);
            m_stkOpItem.push(oItemNum2nd);
            SOperItem oItemNew(E_OP_ITEM_SIGN, ch);
            m_stkOpItem.push(oItemNew);
            m_eState = E_STATE_EXPECT_NUM;
        }
        return E_ERR_OK;
    }
    SOperItem oItemNum1st = m_stkOpItem.top();
    m_stkOpItem.pop();
    SFraction oVal;
    EnumError eErr = doOper(oItemNum1st.oFract, oItemNum2nd.oFract, oItemSign.cSign, oVal);
    if (eErr != E_ERR_OK)
    {
        return eErr;
    }
    SOperItem oItemNum(E_OP_ITEM_NUM, oVal);
    m_stkOpItem.push(oItemNum);
    outputCalcDetail(idx);

    if (ch == ')')
    {
        return reduceStack(ch, idx);
    }
    return try2ReduceStack(ch, idx);
}

void CRationalCalcor::outputCalcDetail(size_t idx)
{
    std::string strDetail;
    std::stack<SOperItem> stkOpItem = m_stkOpItem;
    while (!stkOpItem.empty())
    {
        SOperItem item = stkOpItem.top();
        strDetail = item.toStr() + " " + strDetail;
        stkOpItem.pop();
    }
    if (idx != 0)
    {
        strDetail += m_strFormula.substr(idx);
    }
    printf(" = %s\n", strDetail.c_str());
}
