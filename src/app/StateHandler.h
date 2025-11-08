/* 
 * Copyright (c) 2022-2025, Norbert Schulz
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

#ifndef STATEHANDLER_H_INCLUDED
#define STATEHANDLER_H_INCLUDED

#include "app/IState.h"

namespace app
{
    /** Handler for the various IState instances of the application
     */
    class StateHandler
    {
        public:    
            explicit StateHandler(IState& initialState);
            ~StateHandler() {}

            /** Transition to new state after processing
             * 
             * @param newState State to transision to 
             */
            void setState(IState& newState);

            /** Execute one cycle in the state machine
             */
    	    void process();

        private:
            IState * m_currentState; /**< active (current) state     */
            IState * m_pendingState; /**< pending state to change to */


            StateHandler(const StateHandler&);
            StateHandler& operator=(const StateHandler&);
    };

    inline void StateHandler::setState(IState& newState)
    {
        m_pendingState = &newState;
    }
}

#endif /* STATEHANDLER_H_INCLUDED */