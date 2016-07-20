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

#endif /* MACROUTILS_H_ */