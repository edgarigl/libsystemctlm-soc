/*
 * Copyright (c) 2023 Zero ASIC
 * Written by Edgar E. Iglesias.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef SIGNALS_UMI_H__
#define SIGNALS_UMI_H__

#include "test-modules/signals-common.h"

template
<int DATA_WIDTH>
class UMISignals : public sc_core::sc_module
{
public:
	sc_signal<bool> valid;
	sc_signal<sc_bv<32> > cmd;
	sc_signal<sc_bv<64> > srcaddr;
	sc_signal<sc_bv<64> > dstaddr;
	sc_signal<sc_bv<DATA_WIDTH> > data;
	sc_signal<bool> ready;

	template<typename T>
	void connect(T &dev, const char *prefix)
	{
        signal_connect(&dev, prefix, valid);
        signal_connect(&dev, prefix, cmd);
        signal_connect(&dev, prefix, srcaddr);
        signal_connect(&dev, prefix, dstaddr);
        signal_connect(&dev, prefix, data);
        signal_connect(&dev, prefix, ready);
	}

	void Trace(sc_trace_file *f)
	{
		/* Write data channel.  */
		sc_trace(f, valid, valid.name());
		sc_trace(f, cmd, cmd.name());
		sc_trace(f, srcaddr, srcaddr.name());
		sc_trace(f, dstaddr, dstaddr.name());
		sc_trace(f, data, data.name());
		sc_trace(f, ready, ready.name());
	}

	UMISignals(sc_core::sc_module_name name) :
		sc_module(name),

		valid("valid"),
		cmd("cmd"),
		srcaddr("srcaddr"),
		dstaddr("dstaddr"),
		data("data"),
		ready("ready")
	{}

private:
};
#endif
