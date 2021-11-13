#include "EaseClass.h"
#include<math.h>




Easing::Easing()
{
	maxTimer = 1;
	timer = 0;
	rate = 0.0;
}

double Easing::EaseCalc(int InOrOut, int type, const int &rate, const int &max)
{
	switch (InOrOut)
	{
	case In:
		switch (type)
		{
		case Sine:
			return easeInSine(rate, max);
			break;
		case Quad:
			return easeInQuad(rate, max);
			break;
		case Cubic:
			return easeInCubic(rate, max);
			break;
		case Quart:
			return easeInQuart(rate, max);
			break;
		case Quint:
			return easeInQuint(rate, max);
			break;
		case Expo:
			return easeInExpo(rate, max);
			break;
		case Circ:
			return easeInCirc(rate, max);
			break;
		case Back:
			return easeInBack(rate, max);
			break;
		case Elastic:
			return easeInElastic(rate, max);
			break;
		case Bounce:
			return easeInBounce(rate, max);
			break;
		default:
			break;
		}
		break;
	case Out:
		switch (type)
		{
		case Sine:
			return easeOutSine(rate, max);
			break;
		case Quad:
			return easeOutQuad(rate, max);
			break;
		case Cubic:
			return easeOutCubic(rate, max);
			break;
		case Quart:
			return easeOutQuart(rate, max);
			break;
		case Quint:
			return easeOutQuint(rate, max);
			break;
		case Expo:
			return easeOutExpo(rate, max);
			break;
		case Circ:
			return easeOutCirc(rate, max);
			break;
		case Back:
			return easeOutBack(rate, max);
			break;
		case Elastic:
			return easeOutElastic(rate, max);
			break;
		case Bounce:
			return easeOutBounce(rate, max);
			break;
		default:
			break;
		}
		break;
	case InOut:
		switch (type)
		{
		case Sine:
			return easeinOutSine(rate, max);
			break;
		case Quad:
			return easeInOutQuad(rate, max);
			break;
		case Cubic:
			return easeInOutCubic(rate, max);
			break;
		case Quart:
			return easeInOutQuart(rate, max);
			break;
		case Quint:
			return easeInOutQuint(rate, max);
			break;
		case Expo:
			return easeInOutExpo(rate, max);
			break;
		case Circ:
			return easeInOutCirc(rate, max);
			break;
		case Back:
			return easeInOutBack(rate, max);
			break;
		case Elastic:
			return easeInOutElastic(rate, max);
			break;
		case Bounce:
			return easeInOutBounce(rate, max);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return 0.0;
}

double Easing::Do(int InOrOut, int type)
{
	if (timer >= maxTimer) return 1.0;

	timer += 1;

	rate = EaseCalc(InOrOut, type, timer, maxTimer);
	return rate;
}

double Easing::Linear()
{
	if (timer >= maxTimer) return 1.0;

	timer += 1;

	rate = static_cast<double>(timer) / maxTimer;
	return rate;
}

void Easing::Init(int max)
{
	maxTimer = max;
	timer = 0;
}

bool Easing::IsEnd()
{
	if (timer < maxTimer)	return false;
	return true;
}

double Easing::Read()
{
	return rate;
}

void Easing::Reset()
{
	timer = 0;
	rate = 0.0;
}

double Easing::easeBasic(const int &timer, const int &maxTimer)
{

	if (timer >= maxTimer) return 1.0;

	return static_cast<double>(timer) / maxTimer;

}

//Sine(1)
//easeInSine(ゆっくり→速い)
double Easing::easeInSine(const int &timer, const int &maxTimer)
{
	//変化率
	double changeRate;
	double PI = 3.14159265;

	changeRate = easeBasic(timer, maxTimer);
	return 1 - cos((changeRate * PI) / 2);
}

//easeOutSine(速い→ゆっくり)
double Easing::easeOutSine(const int &timer, const int &maxTimer)
{
	//変化率
	double changeRate;
	double PI = 3.14159265;

	changeRate = easeBasic(timer, maxTimer);
	return sin((changeRate * PI) / 2);

}

//easeInOutSine(ゆっくり→速い→ゆっくり)
double Easing::easeinOutSine(const int &timer, const int &maxTimer)
{
	//変化率
	double changeRate;
	double PI = 3.14159265;

	changeRate = easeBasic(timer, maxTimer);
	return -(cos(PI * changeRate) - 1) / 2;

}


//Quad(2乗)
//easeInQuad(ゆっくり→速い)
double Easing::easeInQuad(const int &timer, const int &maxTimer)
{
	//変化率
	double changeRate;
	//ヌルチェック済み

	changeRate = easeBasic(timer, maxTimer);
	return changeRate * changeRate;

}

//easeOutQuad(速い→ゆっくり)
double Easing::easeOutQuad(const int &timer, const int &maxTimer)
{
	//変化率
	double changeRate;
	changeRate = easeBasic(timer, maxTimer);
	return 1 - (1 - changeRate) * (1 - changeRate);

}

//easeInOutQuad(ゆっくり→速い→ゆっくり)
double Easing::easeInOutQuad(const int &timer, const int &maxTimer)
{
	//変化率
	double changeRate;
	changeRate = easeBasic(timer, maxTimer);
	return				 changeRate < 0.5 ?
		2 * changeRate * changeRate : 1 - pow(-2 * changeRate + 2, 2) / 2;
}


//Cubic(3乗)
//easeInCubic(ゆっくり→速い)
double Easing::easeInCubic(const int &timer, const int &maxTimer)
{
	//変化率
	double changeRate;
	//ヌルチェック済み

	changeRate = easeBasic(timer, maxTimer);
	return changeRate * changeRate * changeRate;

}

//easeOutCubic(速い→ゆっくり)
double Easing::easeOutCubic(const int &timer, const int &maxTimer)
{
	//変化率
	double changeRate;

	changeRate = easeBasic(timer, maxTimer);
	return 1 - pow(1 - changeRate, 3);

}

//easeInOutCubic(ゆっくり→速い→ゆっくり)
double Easing::easeInOutCubic(const int &timer, const int &maxTimer)
{
	//変化率
	double changeRate;

	changeRate = easeBasic(timer, maxTimer);
	return							  changeRate < 0.5 ?
		4 * changeRate * changeRate * changeRate : 1 - pow(-2 * changeRate + 2, 3) / 2;

}


//Quart(4乗)
//easeInQuart(ゆっくり→速い)
double Easing::easeInQuart(const int &timer, const int &maxTimer)
{
	//変化率
	double changeRate;

	changeRate = easeBasic(timer, maxTimer);
	return changeRate * changeRate * changeRate * changeRate;

}

//easeOutQuart(速い→ゆっくり)
double Easing::easeOutQuart(const int &timer, const int &maxTimer)
{
	//変化率
	double changeRate;

	changeRate = easeBasic(timer, maxTimer);
	return 1 - pow(1 - changeRate, 4);

}

//easeInOutQuart(ゆっくり→速い→ゆっくり)
double Easing::easeInOutQuart(const int &timer, const int &maxTimer)
{
	//変化率
	double changeRate;
	//ヌルチェック済み

	changeRate = easeBasic(timer, maxTimer);
	return										   changeRate < 0.5 ?
		8 * changeRate * changeRate * changeRate * changeRate : 1 - pow(-2 * changeRate + 2, 4) / 2;

}


//Quint(5乗)
//easeInQuint(ゆっくり→速い)
double Easing::easeInQuint(const int &timer, const int &maxTimer)
{
	//変化率
	double changeRate;

	changeRate = easeBasic(timer, maxTimer);
	return changeRate * changeRate * changeRate * changeRate * changeRate;

}

//easeOutQuint(速い→ゆっくり)
double Easing::easeOutQuint(const int &timer, const int &maxTimer)
{
	//変化率
	double changeRate;

	changeRate = easeBasic(timer, maxTimer);
	return 1 - pow(1 - changeRate, 5);

}

//easeInOutQuint(ゆっくり→速い→ゆっくり)
double Easing::easeInOutQuint(const int &timer, const int &maxTimer)
{
	//変化率
	double changeRate;
	//ヌルチェック済み

	changeRate = easeBasic(timer, maxTimer);
	return														 changeRate < 0.5 ?
		16 * changeRate * changeRate * changeRate * changeRate * changeRate : 1 - pow(-2 * changeRate + 2, 5) / 2;

}


//Expo(急カーブ)
//easeInExpo
double Easing::easeInExpo(const int &timer, const int &maxTimer)
{
	//変化率
	double changeRate;

	changeRate = easeBasic(timer, maxTimer);
	return changeRate == 0 ? 0 : pow(2, 10 * changeRate - 10);

}

//easeOutExpo
double Easing::easeOutExpo(const int &timer, const int &maxTimer)
{
	//変化率
	double changeRate;

	changeRate = easeBasic(timer, maxTimer);
	return changeRate == 1 ? 1 : 1 - pow(2, -10 * changeRate);

}

//easeInOutExpo
double Easing::easeInOutExpo(const int &timer, const int &maxTimer)
{
	//変化率
	double changeRate;

	changeRate = easeBasic(timer, maxTimer);
	return						changeRate == 0 ?
		0 : changeRate == 1 ?
		1 : changeRate < 0.5 ?
		pow(2, 20 * changeRate - 10) / 2 : (2 - pow(2, -20 * changeRate + 10)) / 2;

}


//Circ
//easeInCirc
double Easing::easeInCirc(const int &timer, const int &maxTimer)
{
	//変化率
	double changeRate;

	changeRate = easeBasic(timer, maxTimer);
	return 1 - sqrt(1 - pow(changeRate, 2));

}

//easeOutCirc
double Easing::easeOutCirc(const int &timer, const int &maxTimer)
{
	//変化率
	double changeRate;

	changeRate = easeBasic(timer, maxTimer);
	return sqrt(1 - pow(changeRate - 1, 2));

}

//easeInOutCirc
double Easing::easeInOutCirc(const int &timer, const int &maxTimer)
{
	//変化率
	double changeRate;

	changeRate = easeBasic(timer, maxTimer);
	return changeRate < 0.5
		? (1 - sqrt(1 - pow(2 * changeRate, 2))) / 2
		: (sqrt(1 - pow(-2 * changeRate + 2, 2)) + 1) / 2;

}


//Back
//easeInBack
double Easing::easeInBack(const int &timer, const int &maxTimer)
{
	const double c1 = 1.70158;
	const double c3 = c1 + 1;
	//変化率
	double changeRate;

	changeRate = easeBasic(timer, maxTimer);
	return c3 * changeRate * changeRate * changeRate - c1 * changeRate * changeRate;

}

//easeOutBack
double Easing::easeOutBack(const int &timer, const int &maxTimer)
{
	const double c1 = 1.70158;
	const double c3 = c1 + 1;
	//変化率
	double changeRate;

	changeRate = easeBasic(timer, maxTimer);
	return 1 + c3 * pow(changeRate - 1, 3) + c1 * pow(changeRate - 1, 2);

}

//easeInOutBack
double Easing::easeInOutBack(const int &timer, const int &maxTimer)
{
	const double c1 = 1.70158;
	const double c2 = c1 * 1.525;
	//変化率
	double changeRate;

	changeRate = easeBasic(timer, maxTimer);
	return changeRate < 0.5
		? (pow(2 * changeRate, 2) * ((c2 + 1) * 2 * changeRate - c2)) / 2
		: (pow(2 * changeRate - 2, 2) * ((c2 + 1) * (changeRate * 2 - 2) + c2) + 2) / 2;

}


//Elastic
//easeInElastic
double Easing::easeInElastic(const int &timer, const int &maxTimer)
{
	double PI = 3.14159265;
	const double c4 = (2 * PI) / 3;
	//変化率
	double changeRate;

	changeRate = easeBasic(timer, maxTimer);
	return changeRate == 0
		? 0
		: changeRate == 1
		? 1
		: -pow(2, 10 * changeRate - 10) * sin((changeRate * 10 - 10.75) * c4);

}

//easeOutElastic
double Easing::easeOutElastic(const int &timer, const int &maxTimer)
{
	double PI = 3.14159265;
	const double c4 = (2 * PI) / 3;
	//変化率
	double changeRate;

	changeRate = easeBasic(timer, maxTimer);
	return changeRate == 0
		? 0
		: changeRate == 1
		? 1
		: -pow(2, -10 * changeRate) * sin((changeRate * 10 - 0.75) * c4) + 1;

}

//easeInOutElastic
double Easing::easeInOutElastic(const int &timer, const int &maxTimer)
{
	double PI = 3.14159265;
	const double c5 = (2 * PI) / 4.5;
	//変化率
	double changeRate;

	changeRate = easeBasic(timer, maxTimer);
	return changeRate == 0
		? 0
		: changeRate == 1
		? 1
		: changeRate < 0.5
		? -(pow(2, 20 * changeRate - 10) * sin((20 * changeRate - 11.125) * c5)) / 2
		: (pow(2, -20 * changeRate + 10) * sin((20 * changeRate - 11.125) * c5)) / 2 + 1;

}


//Bounce
//easeInBounce
double Easing::easeInBounce(const int &timer, const int &maxTimer)
{
	const double n1 = 7.5625;
	const double d1 = 2.75;
	//変化率
	double changeRate;

	changeRate = 1 - (easeBasic(timer, maxTimer));
	if (changeRate < 1 / d1)
	{
		return 1 - (n1 * changeRate * changeRate);
	}
	else if (changeRate < 2 / d1)
	{
		return 1 - (n1 * (changeRate -= 1.5 / d1) * changeRate + 0.75);//0.11
	}
	else if (changeRate < 2.5 / d1)
	{
		return 1 - (n1 * (changeRate -= 2.25 / d1) * changeRate + 0.9375);//0.1111
	}
	else
	{
		return 1 - (n1 * (changeRate -= 2.625 / d1) * changeRate + 0.984375);//0.111111
	}

}

//easeOutBounce
double Easing::easeOutBounce(const int &timer, const int &maxTimer)
{
	const double n1 = 7.5625;
	const double d1 = 2.75;
	//変化率
	double changeRate;

	changeRate = easeBasic(timer, maxTimer);
	if (changeRate < 1 / d1)
	{
		return n1 * changeRate * changeRate;
	}
	else if (changeRate < 2 / d1)
	{
		return n1 * (changeRate -= 1.5 / d1) * changeRate + 0.75;//0.11
	}
	else if (changeRate < 2.5 / d1)
	{
		return n1 * (changeRate -= 2.25 / d1) * changeRate + 0.9375;//0.1111
	}
	else
	{
		return n1 * (changeRate -= 2.625 / d1) * changeRate + 0.984375;//0.111111
	}

}

//easeInOutBounce
double Easing::easeInOutBounce(const int &timer, const int &maxTimer)
{
	const double n1 = 7.5625;
	const double d1 = 2.75;
	//変化率
	double changeRate;

	changeRate = easeBasic(timer, maxTimer);
	if (changeRate < 0.5)
	{
		changeRate = (1 - 2 * changeRate);
		if (changeRate < 1 / d1)
		{
			return (1 - (n1 * changeRate * changeRate)) / 2;
		}
		else if (changeRate < 2 / d1)
		{
			return (1 - (n1 * (changeRate -= 1.5 / d1) * changeRate + 0.75)) / 2;//0.11
		}
		else if (changeRate < 2.5 / d1)
		{
			return (1 - (n1 * (changeRate -= 2.25 / d1) * changeRate + 0.9375)) / 2;//0.1111
		}
		else
		{
			return (1 - (n1 * (changeRate -= 2.625 / d1) * changeRate + 0.984375)) / 2;//0.111111
		}
	}
	else
	{
		changeRate = (2 * changeRate - 1);
		if (changeRate < 1 / d1)
		{
			return (1 + (n1 * changeRate * changeRate)) / 2;
		}
		else if (changeRate < 2 / d1)
		{
			return (1 + (n1 * (changeRate -= 1.5 / d1) * changeRate + 0.75)) / 2;//0.11
		}
		else if (changeRate < 2.5 / d1)
		{
			return (1 + (n1 * (changeRate -= 2.25 / d1) * changeRate + 0.9375)) / 2;//0.1111
		}
		else
		{
			return (1 + (n1 * (changeRate -= 2.625 / d1) * changeRate + 0.984375)) / 2;//0.111111
		}
	}

}