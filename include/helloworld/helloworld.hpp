/**
 * @file this file defines the initial hello function
 */
#ifndef HELLOWORLD_H
#define HELLOWORLD_H
#include <string>
/**
 * namespace to wrap the whole library
 */
namespace hello
{
/**
 * hello is a function to return a constant string for illustration
 * purpose.
 *
 * @return string that is "hello world!"
 */
std::string hello();
}  // namespace hello
#endif