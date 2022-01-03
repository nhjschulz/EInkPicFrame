/*
 * BSD 3-Clause License
 * 
 * Copyright (c) 2022, Norbert Schulz
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

/** Unittesting of StateHandler */

#include <stdio.h>
#include <unity.h>

#include "app/StateHandler.cpp"
#include "app/BaseState.h"

class StateCallCount : public app::IState
{
    public:
        uint32_t m_countEnter;
        uint32_t m_countProcess;
        uint32_t m_countLeave;


        StateCallCount() :
            m_countEnter(0u),
            m_countProcess(0u),
            m_countLeave(0u)
        {

        }
        virtual void enter()   { ++m_countEnter; }
        virtual void process() { ++m_countProcess; }
        virtual void leave()   { ++m_countLeave; }
};

void test_statehandler_generic(void)
{
    StateCallCount counter;

    app::StateHandler stateHandler(counter);

    TEST_ASSERT_EQUAL(0u, counter.m_countEnter);
    TEST_ASSERT_EQUAL(0u, counter.m_countProcess);
    TEST_ASSERT_EQUAL(0u, counter.m_countLeave);

    stateHandler.process();
    TEST_ASSERT_EQUAL(1u, counter.m_countEnter);
    TEST_ASSERT_EQUAL(1u, counter.m_countProcess);
    TEST_ASSERT_EQUAL(0u, counter.m_countLeave);

    stateHandler.process();
    TEST_ASSERT_EQUAL(1u, counter.m_countEnter);
    TEST_ASSERT_EQUAL(2u, counter.m_countProcess);
    TEST_ASSERT_EQUAL(0u, counter.m_countLeave);
}

void test_statehandler_transition(void)
{
    StateCallCount counterA, counterB;

    app::StateHandler stateHandler(counterA);

    
    TEST_ASSERT_EQUAL(0u, counterA.m_countEnter);
    TEST_ASSERT_EQUAL(0u, counterA.m_countProcess);
    TEST_ASSERT_EQUAL(0u, counterA.m_countLeave);
    TEST_ASSERT_EQUAL(0u, counterB.m_countEnter);
    TEST_ASSERT_EQUAL(0u, counterB.m_countProcess);
    TEST_ASSERT_EQUAL(0u, counterB.m_countLeave);

    stateHandler.process();

    TEST_ASSERT_EQUAL(1u, counterA.m_countEnter);
    TEST_ASSERT_EQUAL(1u, counterA.m_countProcess);
    TEST_ASSERT_EQUAL(0u, counterA.m_countLeave);
    TEST_ASSERT_EQUAL(0u, counterB.m_countEnter);
    TEST_ASSERT_EQUAL(0u, counterB.m_countProcess);
    TEST_ASSERT_EQUAL(0u, counterB.m_countLeave);

    stateHandler.setState(counterB);
    stateHandler.process();

    TEST_ASSERT_EQUAL(1u, counterA.m_countEnter);
    TEST_ASSERT_EQUAL(1u, counterA.m_countProcess);
    TEST_ASSERT_EQUAL(1u, counterA.m_countLeave);
    TEST_ASSERT_EQUAL(1u, counterB.m_countEnter);
    TEST_ASSERT_EQUAL(1u, counterB.m_countProcess);
    TEST_ASSERT_EQUAL(0u, counterB.m_countLeave);

    stateHandler.setState(counterA);
    stateHandler.process();

    TEST_ASSERT_EQUAL(2u, counterA.m_countEnter);
    TEST_ASSERT_EQUAL(2u, counterA.m_countProcess);
    TEST_ASSERT_EQUAL(1u, counterA.m_countLeave);
    TEST_ASSERT_EQUAL(1u, counterB.m_countEnter);
    TEST_ASSERT_EQUAL(1u, counterB.m_countProcess);
    TEST_ASSERT_EQUAL(1u, counterB.m_countLeave);
}
