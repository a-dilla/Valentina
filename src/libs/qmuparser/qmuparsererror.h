/***************************************************************************************************
 **
 **  Original work Copyright (C) 2013 Ingo Berg
 **  Modified work Copyright 2014 Roman Telezhynskyi <dismine(at)gmail.com>
 **
 **  Permission is hereby granted, free of charge, to any person obtaining a copy of this
 **  software and associated documentation files (the "Software"), to deal in the Software
 **  without restriction, including without limitation the rights to use, copy, modify,
 **  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 **  permit persons to whom the Software is furnished to do so, subject to the following conditions:
 **
 **  The above copyright notice and this permission notice shall be included in all copies or
 **  substantial portions of the Software.
 **
 **  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 **  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 **  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 **  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 **  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 **
 ******************************************************************************************************/

#ifndef QMUPARSERERROR_H
#define QMUPARSERERROR_H

#include "qmuparser_global.h"
#include <sstream>
#include <QException>

#include "qmuparserdef.h"

/** @file
    @brief This file defines the error class used by the parser.
*/

namespace qmu
{

/** @brief Error codes. */
enum EErrorCodes
{
    // Formula syntax errors
    ecUNEXPECTED_OPERATOR    = 0,  ///< Unexpected binary operator found
    ecUNASSIGNABLE_TOKEN     = 1,  ///< Token cant be identified.
    ecUNEXPECTED_EOF         = 2,  ///< Unexpected end of formula. (Example: "2+sin(")
    ecUNEXPECTED_ARG_SEP     = 3,  ///< An unexpected comma has been found. (Example: "1,23")
    ecUNEXPECTED_ARG         = 4,  ///< An unexpected argument has been found
    ecUNEXPECTED_VAL         = 5,  ///< An unexpected value token has been found
    ecUNEXPECTED_VAR         = 6,  ///< An unexpected variable token has been found
    ecUNEXPECTED_PARENS      = 7,  ///< Unexpected Parenthesis, opening or closing
    ecUNEXPECTED_STR         = 8,  ///< A string has been found at an inapropriate position
    ecSTRING_EXPECTED        = 9,  ///< A string function has been called with a different type of argument
    ecVAL_EXPECTED           = 10, ///< A numerical function has been called with a non value type of argument
    ecMISSING_PARENS         = 11, ///< Missing parens. (Example: "3*sin(3")
    ecUNEXPECTED_FUN         = 12, ///< Unexpected function found. (Example: "sin(8)cos(9)")
    ecUNTERMINATED_STRING    = 13, ///< unterminated string constant. (Example: "3*valueof("hello)")
    ecTOO_MANY_PARAMS        = 14, ///< Too many function parameters
    ecTOO_FEW_PARAMS         = 15, ///< Too few function parameters. (Example: "ite(1<2,2)")
    ecOPRT_TYPE_CONFLICT     = 16, ///< binary operators may only be applied to value items of the same type
    ecSTR_RESULT             = 17, ///< result is a string

    // Invalid Parser input Parameters
    ecINVALID_NAME           = 18, ///< Invalid function, variable or constant name.
    ecINVALID_BINOP_IDENT    = 19, ///< Invalid binary operator identifier
    ecINVALID_INFIX_IDENT    = 20, ///< Invalid function, variable or constant name.
    ecINVALID_POSTFIX_IDENT  = 21, ///< Invalid function, variable or constant name.

    ecBUILTIN_OVERLOAD       = 22, ///< Trying to overload builtin operator
    ecINVALID_FUN_PTR        = 23, ///< Invalid callback function pointer
    ecINVALID_VAR_PTR        = 24, ///< Invalid variable pointer
    ecEMPTY_EXPRESSION       = 25, ///< The Expression is empty
    ecNAME_CONFLICT          = 26, ///< Name conflict
    ecOPT_PRI                = 27, ///< Invalid operator priority
    //
    ecDOMAIN_ERROR           = 28, ///< catch division by zero, sqrt(-1), log(0) (currently unused)
    ecDIV_BY_ZERO            = 29, ///< Division by zero (currently unused)
    ecGENERIC                = 30, ///< Generic error
    ecLOCALE                 = 31, ///< Conflict with current locale

    ecUNEXPECTED_CONDITIONAL = 32,
    ecMISSING_ELSE_CLAUSE    = 33,
    ecMISPLACED_COLON        = 34,

    // internal errors
    ecINTERNAL_ERROR         = 35, ///< Internal error of any kind.

    // The last two are special entries
    ecCOUNT,                      ///< This is no error code, It just stores just the total number of error codes
    ecUNDEFINED              = -1  ///< Undefined message, placeholder to detect unassigned error messages
};

//---------------------------------------------------------------------------
/** @brief A class that handles the error messages.
*/
class QmuParserErrorMsg
{
public:
    typedef QmuParserErrorMsg self_type;

    QmuParserErrorMsg();
    ~QmuParserErrorMsg();

    static const QmuParserErrorMsg& Instance();
    QString operator[] ( unsigned a_iIdx ) const;

private:
    Q_DISABLE_COPY(QmuParserErrorMsg)
    QVector<QString>  m_vErrMsg;  ///< A vector with the predefined error messages
    static const self_type m_Instance;    ///< The instance pointer
};

//---------------------------------------------------------------------------------------------------------------------
// cppcheck-suppress unusedFunction
inline const QmuParserErrorMsg& QmuParserErrorMsg::Instance()
{
    return m_Instance;
}

//---------------------------------------------------------------------------------------------------------------------
inline QString QmuParserErrorMsg::operator[] ( unsigned a_iIdx ) const
{
    return ( a_iIdx < static_cast<unsigned>( m_vErrMsg.size() ) ) ? m_vErrMsg[a_iIdx] : QString();
}

//---------------------------------------------------------------------------
/** @brief Error class of the parser.
    @author Ingo Berg

  Part of the math parser package.
*/
class QMUPARSERSHARED_EXPORT QmuParserError : public QException
{
public:
    QmuParserError();
    explicit QmuParserError ( EErrorCodes a_iErrc );
    explicit QmuParserError ( const QString &sMsg );
    QmuParserError ( EErrorCodes a_iErrc, const QString &sTok, const QString &sFormula = QString(), int a_iPos = -1 );
    QmuParserError ( EErrorCodes a_iErrc, int a_iPos, const QString &sTok );
    QmuParserError ( const QString &a_szMsg, int a_iPos, const QString &sTok = QString() );
    QmuParserError ( const QmuParserError &a_Obj );
    QmuParserError& operator= ( const QmuParserError &a_Obj );
    virtual ~QmuParserError() noexcept (true){}

    void           SetFormula ( const QString &a_strFormula );
    const QString& GetExpr() const;
    const QString& GetMsg() const;
    int            GetPos() const;
    const QString& GetToken() const;
    EErrorCodes    GetCode() const;
    virtual void   raise() const;
    virtual QmuParserError *clone() const;
private:
    QString m_sMsg;      ///< The message string
    QString m_sExpr;     ///< Formula string
    QString m_sTok;      ///< Token related with the error
    int     m_iPos;      ///< Formula position related to the error
    EErrorCodes m_iErrc; ///< Error code
    const QmuParserErrorMsg &m_ErrMsg;
    /**
     * @brief Replace all ocuurences of a substring with another string.
     */
    static void ReplaceSubString ( QString &strSource, const QString &strFind, const QString &strReplaceWith );
    void Reset();
};

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief raise method raise for exception
 */
inline void QmuParserError::raise() const
{
    throw *this;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief clone clone exception
 * @return new exception
 */
inline QmuParserError *QmuParserError::clone() const
{
    return new QmuParserError(*this);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Set the expression related to this error.
 */
inline void QmuParserError::SetFormula ( const QString &a_strFormula )
{
    m_sExpr = a_strFormula;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief gets the expression related tp this error.
 */
inline const QString& QmuParserError::GetExpr() const
{
    return m_sExpr;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Returns the message string for this error.
 */
inline const QString& QmuParserError::GetMsg() const
{
    return m_sMsg;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Return the formula position related to the error.
 *
 * If the error is not related to a distinct position this will return -1
 */
inline int QmuParserError::GetPos() const
{
    return m_iPos;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Return string related with this token (if available).
 */
inline const QString& QmuParserError::GetToken() const
{
    return m_sTok;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Return the error code.
 */
inline EErrorCodes QmuParserError::GetCode() const
{
    return m_iErrc;
}

} // namespace qmu

#endif