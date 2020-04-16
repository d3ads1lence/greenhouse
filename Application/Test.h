/*
 * Test.h
 *
 *  Created on: Apr 15, 2020
 *      Author: user
 */

#pragma once

class Test {
public:
	void Initialize();
	int GetValue() const;

private:
	int _test;
};

inline int Test::GetValue() const
{
	return _test;
}
