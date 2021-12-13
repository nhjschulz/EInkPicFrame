/*
 * BSD 3-Clause License
 * 
 * Copyright (c) 2021, Norbert Schulz
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** Unittesting of Queue.h */

#include <unity.h>
#include "Queue.h"

void test_queue_generic(void) 
{
    Queue<uint8_t, 11> q;

    TEST_ASSERT_EQUAL(true, q.isEmpty());
    TEST_ASSERT_EQUAL(false, q.isFull());
    TEST_ASSERT_EQUAL(11, q.available());
    TEST_ASSERT_EQUAL(0, q.used());

    TEST_ASSERT_EQUAL(true, q.put(0));
    TEST_ASSERT_EQUAL(false, q.isEmpty());
    TEST_ASSERT_EQUAL(false, q.isFull());
    TEST_ASSERT_EQUAL(10, q.available());
    TEST_ASSERT_EQUAL(1, q.used());
    
    q.clear();
    TEST_ASSERT_EQUAL(true, q.isEmpty());
    TEST_ASSERT_EQUAL(false, q.isFull());
    TEST_ASSERT_EQUAL(11, q.available());
    TEST_ASSERT_EQUAL(0, q.used());

    for (uint8_t val(1); val <= 11u; ++ val)
    {
        TEST_ASSERT_EQUAL(true, q.put(val));
        TEST_ASSERT_EQUAL(false, q.isEmpty());
        TEST_ASSERT_EQUAL(val == 11u, q.isFull());
        TEST_ASSERT_EQUAL(11u - val, q.available());
        TEST_ASSERT_EQUAL(val, q.used());
    }

    TEST_ASSERT_EQUAL(false, q.put(0x55)); // fail on full

    for (uint8_t val(1); val <= 11u; ++ val)
    {
        uint8_t getVal(0xFF);
        TEST_ASSERT_EQUAL(true, q.get(getVal));
        TEST_ASSERT_EQUAL(val, getVal);
    }

    uint8_t dummy(0xFF);
    TEST_ASSERT_EQUAL(false, q.get(dummy));  // empty

}

/** Test the corner casse of a one byte queue */
void test_queue_1_element(void) 
{
    Queue<uint8_t, 1> singleByteQueue;

    TEST_ASSERT_EQUAL(true, singleByteQueue.isEmpty());
    TEST_ASSERT_EQUAL(false, singleByteQueue.isFull());
    TEST_ASSERT_EQUAL(1, singleByteQueue.available());
    TEST_ASSERT_EQUAL(0, singleByteQueue.used());

    TEST_ASSERT_EQUAL(true, singleByteQueue.put(16u));

    TEST_ASSERT_EQUAL(false, singleByteQueue.isEmpty());
    TEST_ASSERT_EQUAL(true, singleByteQueue.isFull());
    TEST_ASSERT_EQUAL(0, singleByteQueue.available());
    TEST_ASSERT_EQUAL(1, singleByteQueue.used());

    TEST_ASSERT_EQUAL(false, singleByteQueue.put(0x04));  // discarded on queue full
    
    uint8_t expect_16(0);
    TEST_ASSERT_EQUAL(true, singleByteQueue.get(expect_16));
    TEST_ASSERT_EQUAL(16u, expect_16);
    TEST_ASSERT_EQUAL(false, singleByteQueue.get(expect_16)); // empty 

}    
    
int main(int argc, char **argv)
 {
    UNITY_BEGIN();

    RUN_TEST(test_queue_generic);
    RUN_TEST(test_queue_1_element);
    
    UNITY_END();

    return 0;
}
