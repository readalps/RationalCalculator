#include <stdio.h>
#include <iostream>
#include "RationalCalcor.h"

void showErrInfo(EnumError eErr)
{
    if (eErr == E_ERR_EMPTY)
    {
        printf("The expression is empty.\n\n");
    }
    else if (eErr == E_ERR_INVALID)
    {
        printf("The expression is invalid.\n\n");
    }
    else if (eErr == E_ERR_PAREN_UNMATCHED)
    {
        printf("The expression has unmatched parenthesis.\n\n");
    }
    else if (eErr == E_ERR_ZERO_DENOMINATOR)
    {
        printf("The expression can't be calculated when meeting zero denominator.\n\n");
    }
}

void outputFract(const SFraction& oVal)
{
    printf(" = %s\n\n", oVal.toStr(true).c_str());
}

int main(int argc, char* argv[])
{
    printf("Rational Calculator version 1.0 by Read Alps\n\n");
    printf("Hint: ` denotes the position where the repeating part of a recurring decimal starts.\n\n");
    while (true)
    {
        printf("Please input a rational expression to calculate its value or input q to quit:\n\n ");
        std::string strInput;
        std::getline(std::cin, strInput);
        if (strInput == "q")
        {
            break;
        }
        CRationalCalcor oCalc(strInput);
        EnumError eErr = oCalc.calcIt();
        if (eErr == E_ERR_OK)
        {
            SFraction oVal;
            eErr = oCalc.getResult(oVal);
            if (eErr == E_ERR_OK)
            {
                outputFract(oVal);
                continue;
            }
        }
        showErrInfo(eErr);
    }
    return 0;
}
