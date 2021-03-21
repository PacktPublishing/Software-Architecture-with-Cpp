#pragma once

// the @file annotation is needed for Doxygen to document the free
// functions in this file
/**
 * @file
 * @brief The main entry points of our demo
 */

/**
 * A unit of performable work
 */
struct Payload {
  /**
   * The actual amount of work to perform
   */
  int amount;
};

/**
   @brief Performs really important work
   @param payload  the descriptor of work to be performed
 */
void perform_work(struct Payload payload);
