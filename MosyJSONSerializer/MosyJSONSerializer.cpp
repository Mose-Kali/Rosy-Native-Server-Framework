#include "pch.h"
#include "MosyJSONSerializer.h"

int MosyJSONSerializer::GetNextPair(int off, wchar_t Type)
{
	int Index = JSONString.find(Type, off);
	int Big, Mid, Sml;
	bool inString = false;
	for (int i = Index + 1; i < JSONString.length(); i++)
	{
		if (JSONString[i] == L'{')
		{
			if (!inString)
			{
				Big++;
			}
		}
		else if (JSONString[i] == L'}')
		{
			if (!inString)
			{
				if (Big == 0 && Mid == 0 && Sml == 0)
				{
					return i;
				}
			}
			Big--;
		}
		else if (JSONString[i] == L'[')
		{
			if (!inString)
			{
				Mid++;
			}
		}
		else if (JSONString[i] == L']')
		{
			if (!inString)
			{
				if (Big == 0 && Mid == 0 && Sml == 0)
				{
					return i;
				}
				Mid--;
			}
		}
		else if (JSONString[i] == L'(')
		{
			if (!inString)
			{
				Sml++;
			}
		}
		else if (JSONString[i] == L')')
		{
			if (!inString)
			{
				if (Big == 0 && Mid == 0 && Sml == 0)
				{
					return i;
				}
				Sml--;
			}
		}
		else if (JSONString[i] == L'\"')
		{
			inString = !inString;
		}
	}
	return -1;
}

void MosyJSONSerializer::AddValue(MosyValue Key, MosyValue Value)
{
	wstring KeyPath = Key.GetString();
	vector<wstring> KeyPathv;
	int Last = -1;
	int Start = KeyPath.find(L"\\");
	wstring Parent = L"";
	wstring Grand = L"";
	Last = -1;
	while (Start != KeyPath.npos)
	{
		wstring RootKey;
		RootKey = KeyPath.substr(Last + 1, Start - Last - 1);
		KeyPathv.push_back(RootKey);
		Last = Start;
		Start = KeyPath.find(L"\\", Last + 1);
	}
	wstring RootKey;
	RootKey = KeyPath.substr(Last + 1, KeyPath.length());
	KeyPathv.push_back(RootKey);
	int i;
	for (i = 0; i < KeyPathv.size(); i++)
	{
		wstring RootKey = KeyPathv[i];
		bool Found = false;
		for (int j = 0; j < Tree.size(); j++)
		{
			if (Tree[j].Key == RootKey && Tree[j].deep == i + 1 && Tree[i].Parent == Parent)
			{
				Tree[j].isLeaf = false;
				Found = true;
				break;
			}
		}
		if (!Found)
		{
			Node node;
			node.deep = i + 1;
			node.isArray = false;
			node.isLeaf = false;
			node.Key = RootKey;
			node.Parent = Parent;
			if (i + 1 >= KeyPathv.size())
			{
				node.isLeaf = true;
				node.Value.push_back(Value.GetString());
			}
			Tree.push_back(node);
			MaxDeep = max(i + 1, MaxDeep);
		}
		Parent = RootKey;
	}
}

void MosyJSONSerializer::AddMap(MosyValue Key, map<wstring, MosyValue> Value)
{
	for (map<wstring, MosyValue>::iterator it = Value.begin(); it != Value.end(); it++)
	{
		AddValue(Key.GetString() + L"\\" + it->first, it->second);
	}
}

void MosyJSONSerializer::AddArray(MosyValue Key, vector<MosyValue> Value, bool AddSpr)
{
	wstring KeyPath = Key.GetString();
	vector<wstring> KeyPathv;
	int Last = -1;
	int Start = KeyPath.find(L"\\");
	wstring Parent = L"";
	wstring Grand = L"";
	Last = -1;
	while (Start != KeyPath.npos)
	{
		wstring RootKey;
		RootKey = KeyPath.substr(Last + 1, Start - Last - 1);
		KeyPathv.push_back(RootKey);
		Last = Start;
		Start = KeyPath.find(L"\\", Last + 1);
	}
	wstring RootKey;
	RootKey = KeyPath.substr(Last + 1, KeyPath.length());
	KeyPathv.push_back(RootKey);
	int i;
	for (i = 0; i < KeyPathv.size(); i++)
	{
		wstring RootKey = KeyPathv[i];
		bool Found = false;
		for (int j = 0; j < Tree.size(); j++)
		{
			if (Tree[j].Key == RootKey && Tree[j].deep == i + 1 && Tree[i].Parent == Parent)
			{
				Tree[j].isLeaf = false;
				Found = true;
				break;
			}
		}
		if (!Found)
		{
			Node node;
			node.deep = i + 1;
			node.isArray = false;
			node.isLeaf = false;
			node.Key = RootKey;
			node.Parent = Parent;
			if (i + 1 >= KeyPathv.size())
			{
				node.isArray = true;
				vector<wstring> Val;
				for (vector<MosyValue>::iterator it = Value.begin(); it != Value.end(); it++)
				{
					Val.push_back(it->GetString());
				}
				node.isLeaf = AddSpr;
				node.Value = Val;
			}
			Tree.push_back(node);
			MaxDeep = max(i + 1, MaxDeep);
		}
		Parent = RootKey;
	}

}

void MosyJSONSerializer::AddSet(MosyValue Key, vector<map<wstring, MosyValue>> ValueSet)
{
	wstring KeyPath = Key.GetString();
	vector<wstring> KeyPathv;
	int Last = -1;
	int Start = KeyPath.find(L"\\");
	wstring Parent = L"";
	wstring Grand = L"";
	Last = -1;
	while (Start != KeyPath.npos)
	{
		wstring RootKey;
		RootKey = KeyPath.substr(Last + 1, Start - Last - 1);
		KeyPathv.push_back(RootKey);
		Last = Start;
		Start = KeyPath.find(L"\\", Last + 1);
	}
	wstring RootKey;
	RootKey = KeyPath.substr(Last + 1, KeyPath.length());
	KeyPathv.push_back(RootKey);
	vector<MosyValue> Val;
	wstring strKay = Key.GetString().substr(Key.GetString().find_last_of(L"\\") + 1, Key.GetString().length());
	for (int i = 0; i < ValueSet.size(); i++)
	{
		MosyJSONSerializer Serializer;
		Serializer.AddMap(MosyValue(strKay), ValueSet[i]);
		Val.push_back(MosyValue(Serializer.GetJSONString().GetString()));
	}
	AddArray(Key, Val);
}

void MosyJSONSerializer::Remove(MosyValue Key)
{
	Remove(Key, 1);
}

void MosyJSONSerializer::Remove(MosyValue Key, int Deep)
{
	queue<wstring> Bfs;
	queue<int> DepStk;
	DepStk.push(Deep);
	Bfs.push(Key.GetString());
	int deep = Deep;
	while (!Bfs.empty())
	{
		wstring Target = Bfs.front();
		Bfs.pop();
		deep = DepStk.front();
		DepStk.pop();
		bool Leaf = false;
		for (vector<Node>::iterator it = Tree.begin(); it != Tree.end(); it++)
		{
			if (it->Key == Target && it->deep == deep)
			{
				Leaf = it->isLeaf;
				Tree.erase(it);
				break;
			}
		}//
		//2764737815
		if (!Leaf)
		{
			for (int i = 0; i < Tree.size(); i++)
			{
				if (Tree[i].Parent == Target && Tree[i].deep == deep + 1)
				{
					Bfs.push(Tree[i].Key);
					DepStk.push(Tree[i].deep);
				}
			}
		}
	}
}

void MosyJSONSerializer::Clear()
{
	Tree.clear();
}

bool MosyJSONSerializer::isEmpty()
{
	return Tree.empty();
}

int MosyJSONSerializer::GetObjectCount()
{
	return Tree.size();
}

MosyValue MosyJSONSerializer::GetJSONString()
{
	stack<wstring> Parents;
	stack<int> Starts;
	stack<int> Deeps;
	stack<bool> ArraySign;
	Parents.push(L"");
	Starts.push(0);
	Deeps.push(0);
	ArraySign.push(false);
	wstring JSONString = L"{";
	while (!Parents.empty())
	{
		wstring Parent = Parents.top();
		int Start = Starts.top();
		Starts.pop();
		int Deep = Deeps.top();
		bool End = false;
		if (Start >= Tree.size())
		{
			End = true;
		}
		for (int i = Start; i < Tree.size(); i++)
		{
			if (Tree[i].Parent == Parent && Tree[i].deep == Deep + 1)
			{
				if (Tree[i].isLeaf)
				{
					if (Tree[i].isArray)
					{
						JSONString += L"\"" + Tree[i].Key + L"\":[";
						for (int j = 0; j < Tree[i].Value.size() - 1; j++)
						{
							JSONString += L"\"" + Tree[i].Value[j] + L"\",";
						}
						JSONString += L"\"" + Tree[i].Value[Tree[i].Value.size() - 1] + L"\"";
						JSONString += L"],";
					}
					else
					{
						JSONString += L"\"" + Tree[i].Key + L"\":\"" + Tree[i].Value[0] + L"\",";
					}
					Starts.push(i + 1);
				}
				else
				{
					if (Tree[i].isArray)
					{
						JSONString += L"\"" + Tree[i].Key + L"\":[";
						for (int j = 0; j < Tree[i].Value.size() - 1; j++)
						{
							JSONString += Tree[i].Value[j] + L",";
						}
						JSONString += Tree[i].Value[Tree[i].Value.size() - 1];
						JSONString += L"],";
					}
					else
					{
						Parents.push(Tree[i].Key);
						Starts.push(i + 1);
						Starts.push(0);
						Deeps.push(Deep + 1);
						JSONString += L"\"" + Tree[i].Key + L"\":{";
					}
				}
				break;
			}
			if (i + 1 >= Tree.size())
			{
				End = true;
			}
		}
		if (End)
		{
			Parents.pop();
			Deeps.pop();
			if (JSONString[JSONString.length() - 1] == L',')
			{
				JSONString[JSONString.length() - 1] = L'}';
				JSONString += L",";
			}
			else
			{
				JSONString += L"},";
			}
		}
	}
	JSONString[JSONString.length() - 1] = L'\0';
	return MosyValue(JSONString);
}
