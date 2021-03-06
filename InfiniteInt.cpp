/** 
 * @file InfiniteInt.cpp
 * @brief Implementation for InfiniteInt, an ADT that can store
 *    and perform operations on integers with arbitrarily many digits
 * @author Carl Mofjeld
 * @date 11/23/2020
*/

#include "InfiniteInt.h"

/** InfiniteInt()
 * @brief   Default constructor.
 * @post    This InfiniteInt has a single digit, 0, and isNegative is false.
*/
InfiniteInt::InfiniteInt() : isNegative_(false) {
   digits_.pushFront(0);
}

/** InfiniteInt(int)
 * @brief   Constructs an InfiniteInt that represents the given integer.
 * @param   num   The integer to be converted to an InfiniteInt
 * @post    This InfiniteInt has the same sign and digits as num.
*/
InfiniteInt::InfiniteInt(int num) {
   // Check for INT_MIN
   if (num == INT_MIN) {
      /* Trying to change INT_MIN to positive won't work, given that its
         absolute value is greater than INT_MAX. Here we take off the lowest
         digit to make num small enough to convert in the following block that
         checks for negative inputs */
      digits_.pushFront(abs(num % 10));
      num /= 10;
   }

   // Check for negative input
   if (num < 0) {
      isNegative_ = true;
      num = abs(num);
   } else {
      isNegative_ = false;
   }

   // Push digits one by one to the list of digits.
   // Using a do/while guarantees 0 will be handled correctly.
   do {
      digits_.pushFront(num % 10);
      num /= 10;
   } while (num != 0);
}

/** operator int()
 * @brief   Conversion operator. Returns the number represented by this
 *          InfiniteInt as an integer.
 * @pre     The number represented by this InfiniteInt is not greater than
 *          INT_MAX and not less than INT_MIN.
 * @post    The returned value is equal to the number represented by this
 *          InfiniteInt.
 * @return  The number represented by this InfiniteInt as an integer.
 * @throw   std::range_error if the result would be outside the range
 *          representable by an int.
*/
InfiniteInt::operator int() const {
   // Check for range error
   if ((InfiniteInt(INT_MAX) < *this) ||
      (*this < InfiniteInt(INT_MIN))) {
      throw std::range_error("InfiniteInt outside range representable by int.");
   }

   int result{0}; // The conversion of this InfiniteInt to an int

   // Starting with the highest digit, add the digits one-by-one to the result
   for (auto currentDigit = digits_.begin(); currentDigit != digits_.end(); ++currentDigit) {
      result *= 10;              // Move the previous digit left
      result += *currentDigit;   // Append the current digit
   }

   // Fix the sign, if necessary, and return the result
   if (isNegative_) {
      result *= -1;
   }
   return result;
}

/** numDigits()
 * @brief   Returns the number of decimal digits in the number represented
 *          by this InfiniteInt.
 * @post    The returned value is the number of decimal digits in the number
 *          represented by this InfiniteInt.
 * @return  The number of decimal digits in the number represented by this
 *          InfiniteInt.
*/
int InfiniteInt::numDigits() const {
   return digits_.numEntries();
}

/** operator+(const InfiniteInt&)
 * @brief   Adds the number represented by this InfiniteInt to that represented
 *          by another and returns the result as an InfiniteInt.
 * @param   rhs   The InfiniteInt to add to this one
 * @post    The returned InfiniteInt represents the sum of this InfinteInt's
 *          number and rhs's.
 * @return  InfiniteInt representing the sum of this InfinteInt's number and rhs's.
*/
InfiniteInt InfiniteInt::operator+(const InfiniteInt& rhs) const {
   InfiniteInt result;  // The result of adding the two InfiniteInts

   // Check signs to determine which helper to call and the sign of the result
   if (isNegative_ == rhs.isNegative_) {
      // Same sign - simply add absolute values and set sign to be the same
      result = add(*this, rhs);
      result.isNegative_ = isNegative_;
   } else {
      result = subtract(*this, rhs);
   }

   return result;
}

/** operator-(const InfiniteInt&)
 * @brief   Subtracts the number represented by another InfiniteInt from that represented
 *          by this one and returns the result as an InfiniteInt.
 * @param   rhs   The InfiniteInt to subtract from this one
 * @post    The returned InfiniteInt represents the difference of this InfinteInt's
 *          number and rhs's.
 * @return  InfiniteInt representing the difference of this InfinteInt's number and rhs's.
*/
InfiniteInt InfiniteInt::operator-(const InfiniteInt& rhs) const {
   InfiniteInt result;  // The result of subtracting the two InfiniteInts

   // Check signs to determine which helper to call and the sign of the result
   if (isNegative_ != rhs.isNegative_) {
      // Different signs - add the absolute values
      result = add(*this, rhs);
      result.isNegative_ = isNegative_;
   } else {
      result = subtract(*this, rhs);
   }

   return result;
}

/** operator*(const InfiniteInt&)
 * @brief   Multiplies the number represented by another InfiniteInt with that represented
 *          by this one and returns the result as an InfiniteInt.
 * @param   rhs   The InfiniteInt to multiply with this one
 * @post    The returned InfiniteInt represents the product of this InfinteInt's
 *          number and rhs's.
 * @return  InfiniteInt representing the product of this InfinteInt's number and rhs's.
*/
InfiniteInt InfiniteInt::operator*(const InfiniteInt& rhs) const {
   InfiniteInt result{0};     // The result of multiplying the two InfiniteInts
   int digitResult{0};        // The result of multiplying two digits
   int carry{0};              // The carry value after multiplying two digits
   int numZeroes{0}; // The number of zeroes to add onto partial result (effectively multiplying by powers of 10)

   // Check if either InfiniteInt is zero
   if ((*this == result) || (rhs == result)) {
      return result;
   } else {
      result.digits_.popFront();
   }

   // Multiply each digit in rhs with every digit in lhs
   for (auto rhsCur = rhs.digits_.last(); rhsCur != rhs.digits_.end(); --rhsCur) {
      InfiniteInt partialResult;       // The result of multiplying one digit from rhs with all of lhs
      partialResult.digits_.popFront(); // Remove default zero value

      // Multiply the current digit in rhs with every digit in lhs
      for (auto lhsCur = digits_.last(); lhsCur != digits_.end(); --lhsCur) {
         digitResult = *lhsCur * *rhsCur + carry;           // multiply the digits
         partialResult.digits_.pushFront(digitResult % 10);  // record the result
         carry = digitResult / 10;                          // calculate the carry
      }

      // Check for a final carry
      if (carry > 0) {
         partialResult.digits_.pushFront(carry);
      }
      
      // Add zeroes onto partial result to multiply by powers of ten
      for (int i = 0; i < numZeroes; ++i) {
         partialResult.digits_.pushBack(0);
      }

      // Add to total result and reset/update variables for next digit of rhs
      result = add(result, partialResult);
      carry = 0;
      ++numZeroes;
   }

   // Determine the sign of the result and return it
   result.isNegative_ = isNegative_ != rhs.isNegative_;
   return result;
}

/** add(const InfiniteInt&, const InfiniteInt&)
 * @brief   Helper method to add InfiniteInts. Ignores the sign of both InfiniteInts.
 * @param   rhs   The InfiniteInt to add to this one
 * @post    The returned InfiniteInt represents the sum of the absolute values of
 *          this InfinteInt's number and rhs's.
 * @return  InfiniteInt representing the sum of this InfinteInt's number and rhs's.
*/
InfiniteInt InfiniteInt::add(const InfiniteInt& lhs, const InfiniteInt& rhs) const {
   InfiniteInt result;        // The result of adding the InfiniteInts
   result.digits_.clear();     // Remove default 0 digit
   int partialSum{0};         // The total from summing two digits
   int carry{0};              // The carry value after summing two digits
   auto lhsCur = lhs.digits_.last(); // iterator for lhs starting at ones digit
   auto rhsCur = rhs.digits_.last(); // iterator for rhs starting at ones digit

   // While both IIs have digits, add them one-by-one and record in result
   while (lhsCur != lhs.digits_.end() && rhsCur != rhs.digits_.end()) {
      partialSum = *lhsCur + *rhsCur + carry;   // add the digits
      result.digits_.pushFront(partialSum % 10); // record result
      carry = partialSum / 10;                  // calculate carry

      // Go to next highest digits (ones digit is at the end so we need to decrement)
      --lhsCur;
      --rhsCur;
   }

   // While either II still has digits, add them to the result (accounting for carries)
   while (lhsCur != lhs.digits_.end()) {
      partialSum = *lhsCur + carry;
      result.digits_.pushFront(partialSum % 10);
      carry = partialSum / 10;
      --lhsCur;
   }
   while (rhsCur != rhs.digits_.end()) {
      partialSum = *rhsCur + carry;
      result.digits_.pushFront(partialSum % 10);
      carry = partialSum / 10;
      --rhsCur;
   }

   // Check for a final carry
   if (carry > 0) {
      result.digits_.pushFront(carry);
   }

   return result;
}

/** subtract(const InfiniteInt& rhs)
 * @brief   Helper method to subtract InfiniteInts.
 * @param   lhs   The InfiniteInt being subtracted from
 * @param   rhs   InfiniteInt being subtracted from lhs
 * @post    The returned InfiniteInt represents the difference of lhs's
 *          number and rhs's.
 * @return  InfiniteInt representing the difference of lhs's number and
 *          rhs's.
*/
InfiniteInt InfiniteInt::subtract(const InfiniteInt& lhs, const InfiniteInt& rhs) const {
   // Need to subtract the smaller absolute value from the larger
   InfiniteInt result;           // The result of subtracting the two InfiniteInts
   result.digits_.clear();        // Remove default 0 digit
   InfiniteInt lhsCopy(lhs);     // copy of lhs that can be changed
   InfiniteInt rhsCopy(rhs);     // copy of rhs that can be changed

   // Find largest absolute value and compute the difference
   lhsCopy.isNegative_ = false;
   rhsCopy.isNegative_ = false;
   const InfiniteInt& larger = lhsCopy < rhsCopy ? rhsCopy : lhsCopy;
   const InfiniteInt& smaller = &larger == &lhsCopy ? rhsCopy : lhsCopy;

   int partialDiff{0};        // The total from subtracting two digits
   int borrow{0};             // The borrow value after subtracting two digits
   auto largerCur = larger.digits_.last();   // iterator for top InfiniteInt
   auto smallerCur = smaller.digits_.last(); // iterator for bottom InfiniteInt

   // While both IIs have digits, subtract them one-by-one and record in result
   while (largerCur != larger.digits_.end() && smallerCur != smaller.digits_.end()) {
      partialDiff = *largerCur - *smallerCur - borrow;   // subtract the digits

      // Check if borrow needed
      if (partialDiff < 0) {
         partialDiff += 10;
         borrow = 1;
      } else {
         borrow = 0;
      }

      // Record result
      result.digits_.pushFront(partialDiff);

      // Go to next highest digits (ones digit is at the end so we need to decrement)
      --largerCur;
      --smallerCur;
   }

   // While lhs still has digits, add them to the result (accounting for borrows)
   while (largerCur != larger.digits_.end()) {
      partialDiff = *largerCur - borrow;
      if (partialDiff < 0) {
         partialDiff += 10;
         borrow = 1;
      } else {
         borrow = 0;
      }
      result.digits_.pushFront(partialDiff);
      --largerCur;
   }


   // Remove any leading zeroes and fix the sign of the result, if necessary
   result.removeLeadingZeroes();
   if (result != InfiniteInt(0)) {
      if ((lhs.isNegative_ && (&larger == &lhsCopy)) ||
         (!lhs.isNegative_ && (&larger == &rhsCopy))) {
         result.isNegative_ = true;
      }
   }  // Otherwise result should be positive, which it is by default
   
   return result;
}

/** operator==(const InfiniteInt& rhs)
 * @brief   Equality operator. Checks if this InfiniteInt represents the same integer
 *          as another.
 * @param   rhs   The InfiniteInt being compared to.
 * @post    The returned value is true if both InfiniteInts have the same sign and the
 *          same digits in the same order. Otherwise, it is false.
 * @return  True if this InfiniteInt is equal to rhs and false otherwise.
*/
bool InfiniteInt::operator==(const InfiniteInt& rhs) const {
   // Check number for difference in number of digits or sign
   if ((numDigits() != rhs.numDigits()) || (isNegative_ != rhs.isNegative_)) {
      // Different number of digits or sign - can't be equal
      return false;
   }

   // Check the digits one-by-one, looking for a difference
   auto lhsCur = digits_.begin();     // iterator for this II
   auto rhsCur = rhs.digits_.begin(); // iterator for rhs
   while (lhsCur != digits_.end() && rhsCur != rhs.digits_.end()) {
      if (*lhsCur != *rhsCur) {
         // Difference found - not equal
         return false;
      }
      ++lhsCur;
      ++rhsCur;
   }

   // No difference found above - must be equal
   return true;
}

/** operator!=(const InfiniteInt& rhs)
 * @brief   Inequality operator. Checks if this InfiniteInt represents the same integer
 *          as another.
 * @param   rhs   The InfiniteInt being compared to.
 * @post    The returned value is false if both InfiniteInts have the same sign and the
 *          same digits in the same order. Otherwise, it is true.
 * @return  False if this InfiniteInt is equal to rhs and true otherwise.
*/
bool InfiniteInt::operator!=(const InfiniteInt& rhs) const {
   return !operator==(rhs);
}

/** operator<(const InfiniteInt& rhs)
 * @brief   Less-than operator. Checks if the integer this InfiniteInt represents
 *          is less than that of another.
 * @param   rhs   The InfiniteInt being compared to.
 * @post    The returned value is true if this InfiniteInt's integer is less than
 *          rhs's and false otherwise.
 * @return  True if this InfiniteInt is less than rhs and false otherwise.
*/
bool InfiniteInt::operator<(const InfiniteInt& rhs) const {
   // Check for differences in sign
   if (!isNegative_ && rhs.isNegative_) {
      return false;
   } else if (isNegative_ && !rhs.isNegative_) {
      return true;
   }

   // Check for differences in # of digits (signs are the same)
   if (numDigits() != rhs.numDigits()) {
      if (isNegative_) {
         if (numDigits() > rhs.numDigits()) {
            return true;
         } else if (numDigits() < rhs.numDigits()) {
            return false;
         }
      } else if (!isNegative_) {
         if (numDigits() > rhs.numDigits()) {
            return false;
         } else if (numDigits() < rhs.numDigits()) {
            return true;
         }
      }
   }

   // Same sign and # of digits - check digits one-by-one, starting with highest
   auto lhsCur = digits_.begin();     // iterator for this II
   auto rhsCur = rhs.digits_.begin(); // iterator for rhs
   while (lhsCur != digits_.end() && rhsCur != rhs.digits_.end()) {
      // Check for lhs < rhs
      if ((!isNegative_ && (*lhsCur > *rhsCur)) ||
         (isNegative_ && (*lhsCur < *rhsCur))) {
         // lhs must be larger
         return false;
      }

      // Check for rhs > lhs
      if ((!isNegative_ && (*lhsCur < *rhsCur)) ||
         (isNegative_ && (*lhsCur > *rhsCur))) {
         // lhs must be larger
         return true;
      }

      // Same digit - advance to the next
      ++lhsCur;
      ++rhsCur;
   }

   // No difference found above - must be equal
   return false;
}

/** removeLeadingZeroes()
 * @brief   Removes any leading zero digits from this InfiniteInt.
 * @post    All leading zero digits, other than the ones digit, have been removed from this InfiniteInt.
*/
void InfiniteInt::removeLeadingZeroes() {
   while ((digits_.numEntries() > 1) && (digits_.front() == 0)) {   // Don't remove the ones digit
      digits_.popFront();
   }
}

/** operator<<(ostream&, const InfiniteInt&)
 * @brief   Outputs a InfiniteInt to an output stream
 * @param   outStream      The stream to print the queue's entries to
 * @param   IIToPrint      The InfiniteInt whose entries are being printed
 * @pre     outStream is not in an error state when the function is called
 * @post    A textual representation of the number represented by this InfiniteInt
 *          has been output to outStream.
 * @return  Reference to the modified stream.
*/
std::ostream& operator<<(std::ostream& outStream, const InfiniteInt& IIToPrint) {
   // Output minus sign, if necessary
   if (IIToPrint.isNegative_) {
      outStream << '-';
   }

   // Output the digits, from highest to lowest
   for (auto iter = IIToPrint.digits_.begin(); iter != IIToPrint.digits_.end(); ++iter) {
      outStream << *iter;
   }

   // Return stream
   return outStream;
}

/** operator>>(istream&, const InfiniteInt&)
 * @brief   Reads digits from an input stream and inputs them into an InfiniteInt
 * @param   inStream    The stream to read from
 * @param   IIToFill    The InfiniteInt to read into
 * @pre     inStream is not in an error state when the function is called
 * @post    All initial whitespace characters in inStream have been discarded.
 *          If the first non-whitespace character is a digit ('0' - '9'),
 *          all consecutive digits immediately following the first have been read
 *          from the stream and IIToFill contains those same digits in the same
 *          order in which they were read. If the first character was '-' followed
 *          by at least one digit, then the InfiniteInt has been set to be negative
 *          and all consecutive digits have been read and stored, as before. In
 *          all other cases, the InfiniteInt is set to zero.
 * @return  Reference to the modified stream.
*/
std::istream& operator>>(std::istream& inStream, InfiniteInt& IIToFill) {
   // Reset the InfiniteInt
   IIToFill.digits_.clear();
   IIToFill.isNegative_ = false;

   // Discard leading whitespace
   inStream >> std::ws;

   // Check for minus sign
   if (inStream.peek() == '-') {
      IIToFill.isNegative_ = true;
      inStream.ignore(1);  // remove '-' from the stream
   }

   // Discard any leading zeroes
   while (inStream.peek() == '0') {
      inStream.ignore(1);
   }

   // Read in digits and store them
   char currentChar;    // latest character read from the stream
   while (inStream.get(currentChar)) {
      if (std::isdigit(currentChar)) {
         IIToFill.digits_.pushBack(currentChar - '0');
      } else {
         // Not a digit - put it back in the stream and stop reading
         inStream.putback(currentChar);
         break;
      }
   }

   // If no digits were read from inStream, set the InfiniteInt to zero
   if (IIToFill.digits_.numEntries() == 0) {
      IIToFill.digits_.pushBack(0);
      
      /* Check whether a leading '-' was read from inStream.
         If so, put it back and set IIToFill to be positive. */
      if (IIToFill.isNegative_) {
         inStream.putback('-');
         IIToFill.isNegative_ = false;
      }
   }

   return inStream;
}