//  -------------------------------------------------------------------------
//  Copyright (C) 2019 BMW AG
//  -------------------------------------------------------------------------
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at https://mozilla.org/MPL/2.0/.
//  -------------------------------------------------------------------------

#ifndef RAMSES_FRAMEWORKTESTUTILS_IOSTREAMTESTER_H
#define RAMSES_FRAMEWORKTESTUTILS_IOSTREAMTESTER_H

#include "Collections/IInputStream.h"
#include "Collections/IOutputStream.h"
#include "gtest/gtest.h"
#include <vector>
#include <cassert>
#include <cstring>

namespace ramses_internal
{
    class IOStreamTester : public IInputStream, public IOutputStream
    {
    public:
        virtual IInputStream& read(char* data, uint32_t size) override
        {
            assert(data);
            assert(size > 0);
            assert(m_readPos + size <= m_buffer.size());
            std::memcpy(data, m_buffer.data() + m_readPos, size);
            m_readPos += size;
            return *this;
        }

        virtual EStatus getState() const override
        {
            return EStatus_RAMSES_OK;
        }

        virtual IOutputStream& write(const void* data, const uint32_t size) override
        {
            m_buffer.insert(m_buffer.end(), static_cast<const char*>(data), static_cast<const char*>(data)+size);
            return *this;
        }

    private:
        std::vector<char> m_buffer;
        uint32_t m_readPos = 0;
    };

    class IOStreamTesterBase
    {
    public:
        template <typename T>
        void expectSame(T in, T out = {})
        {
            IOStreamTester io;
            io << in;
            io >> out;
            EXPECT_EQ(in, out);
        }

        template <typename T, typename U>
        void expectSame2(T in, U out = {})
        {
            IOStreamTester io;
            io << in;
            io >> out;
            EXPECT_EQ(in, out);
        }
    };
}

#endif
