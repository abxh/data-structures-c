/**
 * @file macros.h
 * @brief Useful C preprocessor macros.
 *
 * **FAQ**:
 *
 * Why these PASTE/CONCAT/JOIN are defined?
 *
 * These are used to create the symbols (struct names / function names) for different data
 * structure types.
 *
 * Alternatives:
 *   @li Seperate data structure type code for each type used.
 *   @li `void*` types, which are converted at runtime; 
 *   @li Buffers with a data_size worked with at runtime.
 *   @li Macros expanded with type parameters.
 *   @li Use `container_of` tricks.
 */

/**
 * @def PASTE(a,b)
 * @brief Pastes it's two parameters together.
 * @return The two parameters pasted together.
 */
#define PASTE(a, b) a##b

/**
 * @def CONCAT(a,b)
 * @brief First expands it's two parameters, then pastes them together.
 * @return The two parameters pasted together after they are expanded.
 */
#define CONCAT(a, b) PASTE(a, b)

/**
 * @def JOIN(a,b)
 * @brief First expands it's two parameters, then pastes them together with a underscore _ in between.
 * @return The two parameters pasted together with a _ in between after they are expanded.
 */
#define JOIN(a, b) CONCAT(a, CONCAT(_, b))

/**
 * @def IMPLIES(a,b)
 * @brief Macro corresponding to the logical implication operator.
 * @retval true If a is true and b is not false. 
 * @retval false If a is true and b is false.
 */
#define IMPLIES(a, b) (!(a) || (b))
