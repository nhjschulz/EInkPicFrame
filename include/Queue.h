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
#include <stdint.h>

/** Implementation of a fixed size queue that can hold ""S" elements of type "T".
 *  Note: S must be larger then zero to be useful.
 */
template<typename T, uint8_t S>
class Queue
{
    public:
        Queue();

        /** check if queue is empty */
        bool    isEmpty() const;

        /** check if queue is full */
        bool    isFull() const;

        /** get number of free slots */
        uint8_t available() const;

        /** get number of used slots */
        uint8_t used() const;

        /** reset the queue to be empty */
        void    clear();

        /** add element to queue 
         *  @param[in] element  element to copy into queue
         *  @return true of succesfull, false if queue full
         */
        bool    put(const T& element);

        /** get element from queue 
         * @param[out] element  destination for extracted element
         * @return true if element returned, false if queue empty
        */
        bool    get(T& element);

    private:
        static const 
        uint8_t M_LEN = S + 1;       /**< internal consumed space   */

        uint8_t   m_wrIdx;           /**< next write position       */
        uint8_t   m_rdIdx;           /**<next read position         */
        T         m_buffer[M_LEN];   /**< memory for Queue elements */

};

// ---------------------- Iniliners ------------------------------------------

template <class T, uint8_t S> 
Queue<T,S>::Queue() :
    m_wrIdx(0u),
    m_rdIdx(0u)
{
}
 
template <class T, uint8_t S> 
inline uint8_t Queue<T,S>::used() const
{
    uint8_t usedSlots(m_wrIdx);

    if (m_wrIdx >= m_rdIdx)
    {
        usedSlots -= m_rdIdx;
    }
    else
    {
        usedSlots  += M_LEN - m_rdIdx + 1;
    }

    return usedSlots;
}

template <class T, uint8_t S> 
inline uint8_t Queue<T, S>::available() const
{
    return S - this->used();
}

template <class T, uint8_t S> 
inline bool Queue<T, S>::isEmpty() const
{
    return m_wrIdx == m_rdIdx;
}

template <class T, uint8_t S> 
inline bool Queue<T,S>::isFull() const
{
    return S == this->used();
}


template <class T, uint8_t S> 
inline void Queue<T,S>::clear()
{
    m_wrIdx = 0u;
    m_rdIdx = 0u;
}

template <class T, uint8_t S> 
inline bool Queue<T, S>::put(const T& element)
{
    uint8_t next((m_wrIdx + 1) % M_LEN); /**< next insert idx */
    bool    result(false);
    
    if (next != m_rdIdx)
    {
        // queue is not yet full 

        m_buffer[next] = element;
        m_wrIdx = next;
        result = true;
    }

    return result;
}

template <class T, uint8_t S> 
inline bool Queue<T, S>::get(T& element)
{
    bool result(false);

    if (!isEmpty()) 
    {
        uint8_t next((m_rdIdx + 1) % M_LEN); /**< next read idx */

        element = m_buffer[next];
        m_rdIdx = next;
        result  = true;
    }

    return result;
}
