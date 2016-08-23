/*
 * MacroUtils.h
 *
 * Created: 7/20/2016 5:51:12 PM
 *  Author: Sensics
 */

#ifndef MACROUTILS_H_
#define MACROUTILS_H_

/** @brief Utility macro used in stringification of macros. */
#define SVR_STRINGIFY_IMPL(X) #X

/** @brief Utility macro for stringification of macro expansions. */
#define SVR_STRINGIFY(X) SVR_STRINGIFY_IMPL(X)

/** @brief Utility macro used in concatenation. */
#define SVR_CAT_IMPL(X) X

/** @brief Utility macro for token pasting aka concatenation */
#define SVR_CAT(A, B) SVR_CAT_IMPL(A##B)

/** @brief Utility macro for token pasting aka concatenation following macro expansion */
#define SVR_EXPAND_CAT(A, B) SVR_CAT(A, B)

/** @brief Utility macro for token pasting aka concatenation */
#define SVR_CAT3(A, B, C) SVR_CAT_IMPL(A##B##C)

/** @brief Utility macro for token pasting aka concatenation following macro expansion */
#define SVR_EXPAND_CAT3(A, B, C) SVR_CAT3(A, B, C)

/** @brief Utility macro for token pasting aka concatenation */
#define SVR_CAT4(A, B, C, D) SVR_CAT_IMPL(A##B##C##D)

/** @brief Utility macro for token pasting aka concatenation following macro expansion */
#define SVR_EXPAND_CAT4(A, B, C, D) SVR_CAT4(A, B, C, D)

/** @brief Utility macro for token pasting aka concatenation */
#define SVR_CAT5(A, B, C, D, E) SVR_CAT_IMPL(A##B##C##D##E)

/** @brief Utility macro for token pasting aka concatenation following macro expansion */
#define SVR_EXPAND_CAT5(A, B, C, D, E) SVR_CAT5(A, B, C, D, E)

#endif /* MACROUTILS_H_ */