#pragma once

namespace RancUtilities
{
	template <typename T>
	static FString PrintArrayContents(const TArray<T>& Array, bool bReverse = false)
	{
		FString Result = "{";

		if (!bReverse)
		{
			for (int32 i = 0; i < Array.Num(); ++i)
			{
				Result += Array[i].ToString();

				if (i < Array.Num() - 1)
				{
					Result += ", ";
				}
			}
		}
		else
		{
			for (int32 i = Array.Num() - 1; i >= 0; --i)
			{
				Result += Array[i].ToString();

				if (i > 0)
				{
					Result += ", ";
				}
			}
		}

		Result += "}";

		return Result;
	}
}