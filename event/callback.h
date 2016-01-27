/*
 * Copyright (c) 2008-2016 Juli Mallett. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef	EVENT_CALLBACK_H
#define	EVENT_CALLBACK_H

#include <event/action.h>

class CallbackBase;
class Lock;

class CallbackScheduler {
protected:
	CallbackScheduler(void)
	{ }

public:
	virtual ~CallbackScheduler()
	{ }

	virtual void cancel(CallbackBase *) = 0;
	virtual Action *schedule(CallbackBase *) = 0;
};

class CallbackBase : private Action {
	CallbackScheduler *scheduler_;
	Lock *lock_;
	bool scheduled_;
protected:
	CallbackBase(CallbackScheduler *scheduler, Lock *xlock)
	: scheduler_(scheduler),
	  lock_(xlock),
	  scheduled_(false)
	{ }

public:
	virtual ~CallbackBase()
	{ }

public:
	void cancel(void);

	virtual void execute(void) = 0;

	Action *schedule(void);

	Action *scheduled(CallbackScheduler *scheduler)
	{
		/* Pin anything not already pinned.  */
		if (scheduler_ == NULL)
			scheduler_ = scheduler;
		/* It must not be pinned elsewhere.  */
		ASSERT("/callback/base", scheduler_ == scheduler);
		/* It must not already be scheduled.  */
		ASSERT("/callback/base", !scheduled_);
		scheduled_ = true;
		return (this);
	}

	Lock *lock(void) const
	{
		return (lock_);
	}
};

class SimpleCallback : public CallbackBase {
protected:
	SimpleCallback(CallbackScheduler *scheduler, Lock *xlock)
	: CallbackBase(scheduler, xlock)
	{ }

public:
	virtual ~SimpleCallback()
	{ }

	void execute(void)
	{
		(*this)();
	}

protected:
	virtual void operator() (void) = 0;
};

#endif /* !EVENT_CALLBACK_H */
