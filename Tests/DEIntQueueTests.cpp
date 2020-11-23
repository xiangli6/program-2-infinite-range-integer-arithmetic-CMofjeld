/** 
 * @file DEIntQueueTests.cpp
 * @brief Defines catch2 unit tests for DEIntQueue
 * @author Carl Mofjeld
 * @date 11/23/2020
*/

#include "catch.hpp"       // catch2 required header
#include "../DEIntQueue.h" // class being tested
#include <sstream>         // allow testing of queue contents via printing

TEST_CASE("DEIntQueue constructor creates empty queue", "[DEIntQueue") {
   // Setup
   std::stringstream expected{""};  // Expected output from queue 
   std::stringstream actual;        // Actual output from queue

   // Run
   DEIntQueue queue;
   actual << queue;

   // Test
   CHECK(queue.numEntries() == 0);
   CHECK(actual.str() == expected.str());
}

TEST_CASE("DEIntQueue::pushFront adds item to front of empty queue", "[DEIntQueue") {
   // Setup
   std::stringstream expected{"1 "};   // Expected output from queue
   std::stringstream actual;           // Actual output from queue
   DEIntQueue queue;
   REQUIRE(queue.numEntries() == 0);
   
   // Run
   queue.pushFront(1);
   actual << queue;

   // Test
   CHECK(queue.numEntries() == 1);
   CHECK(actual.str() == expected.str());
}

TEST_CASE("DEIntQueue::pushFront adds item to front of non-empty queue", "[DEIntQueue") {
   // Setup
   std::stringstream expected{"2 1 "};   // Expected output from queue
   std::stringstream actual;           // Actual output from queue
   DEIntQueue queue;
   REQUIRE(queue.numEntries() == 0);
   
   // Run
   queue.pushFront(1);
   queue.pushFront(2);
   actual << queue;

   // Test
   CHECK(queue.numEntries() == 2);
   CHECK(actual.str() == expected.str());
}

TEST_CASE("DEIntQueue::front returns the first item in a queue", "[DEIntQueue") {
   // Setup
   DEIntQueue queue;
   
   SECTION("with 1 item in queue") {
      // Run
      queue.pushFront(1);

      // Test
      CHECK(queue.front() == 1);
   }

   SECTION("with >1 item in queue") {
      // Run
      queue.pushFront(1);
      queue.pushFront(2);

      // Test
      CHECK(queue.front() == 2);
   }
}

TEST_CASE("DEIntQueue::front throws an exception for an empty queue", "[DEIntQueue") {
   // Setup
   DEIntQueue queue;
   REQUIRE(queue.numEntries() == 0);
   REQUIRE_THROWS_AS(queue.front(), std::logic_error);
}