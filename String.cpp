#include "String.h"
int length(const char* s)
{
    int i = 0;
    while (s[i] != '\0') { i++; }
    return i;
}
int String::lenght(const char* s)
{
    int i = 0;
    while (s[i] != '\0') { i++; }
    return i;
}
String::String()
{
    str = nullptr;
    len = 0;
    cap = 0;
}
String::String(const char* ptr)
{
    str = nullptr;
    len = lenght(ptr);
    cap = len;
    str = new char[len + 1];
    for (int i = 0; i < len; i++)
    {
        str[i] = ptr[i];
    }
}
String::String(const char c, int s)
{
    len = s;
    cap = s;
    str = new char[len + 1];
    for (int i = 0; i < len; i++)
    {
        str[i] = c;
    }
}
String::String(int n)
{
    this->itos(n);
}
String String::replace_first(char c)
{
    str[0] = c;
    return *this;
}
void String::print() const
{
    for (int i = 0; i < len; i++)
    {
        cout << str[i];
    }
    cout << endl;
}
String::String(const String& b)
{
    str = nullptr;
    len = 0;
    cap = 0;
    len = b.len;
    cap = b.cap;
    str = new char[cap + 1];
    for (int i = 0; i < len; i++)
    {
        str[i] = b.str[i];
    }
}
String& String::operator=(const String& s)
{
    if (this == &s)
        return *this;
    delete[] str;
    len = s.len;
    cap = s.cap;
    str = new char[len + 1];
    for (int i = 0; i < len; i++)
    {
        str[i] = s.str[i];
    }
    str[len] = '\0';
    return *this;
}
String String::operator+(const String& s) const
{
     String res;
     res.len = len + s.len + 1;
     res.cap = res.len + 1;
     res.str = new char[res.cap];
     for (int i = 0; i < len; i++)
     {
         res.str[i] = str[i];
     }
     res.str[len] = ' ';
     for (int i = 0; i < s.len; i++)
     {
         res.str[i + len + 1] = s.str[i];
     }
     res.str[res.len] = '\0';
     return res;
}
int String::stoi(const String& s)
{
    int res = 0;
    for (int i = 0; i < s.len - 1; i++)
    {
        res = res * 10 + (s.str[i] - '0');
    }
    return res;
}
String& String::itos(int n)
{
    int temp = n;
    int s = 0;
    while (temp > 0)
    {
        temp = temp / 10;
        s++;
    }
    len = s + 1;
    cap = s + 1;
    str = new char[len];
    for (int i = s - 1; i > -1; i--)
    {
        int j = n % 10;
        str[i] = '0' + j;
        n = n / 10;
    }
    str[s] = '\0';
    return *this;
}
String String::trim()
{
    int i = len - 1;
    while (str[i] == '\n' || str[i] == '\t' || str[i] == ' ')
    {
        i--;
    }
    len = i + 1;
    str[len] = '\0';
    i = 0;
    while (str[i] == '\n' || str[i] == '\t' || str[i] == ' ')
    {
        i++;
    }
    for (int j = 0; j < len; j++)
    {
        str[j] = str[i + j];
    }
    str[len] = '\0';
    return *this;
}
char String::operator[](int i) const
{
    if (i < len)
    {
        return str[i];
    }
    return '\0';
}
char& String::operator[](int i)
{
    if (i < len)
    {
        return str[i];
    }
}
bool String::is_equal(const String s)
{
    if (len != s.len)
    {
        return false;
    }
    for (int i = 0; i < len; i++)
    {
        if (str[i] != s.str[i])
        {
            return false;
        }
    }
    return true;
}
bool String::is_less(const String s)
{
    for (int i = 0; i < len; i++)
    {
        if (str[i] > s.str[i])
        {
            return false;
        }
    }
    return true;
}
bool String::is_greater(const String s)
{
    for (int i = 0; i < len; i++)
    {
        if (str[i] < s.str[i])
        {
            return false;
        }
    }
    return true;
}
String* String::split(char delim, int& count)const
{
    count = 0;
    for (int i = 0; i < len; i++)
    {
        if (str[i] == delim)
        {
            count++;
        }
    }
    int* arr = new int[count + 1];
    int j = 0, k = 0;
    for (int i = 0; i < len; i++)
    {
        if (str[i] != delim)
        {
            j++;
        }
        else
        {
            arr[k] = j;
            j = 0;
            k++;
        }
    }
    arr[k] = j;
    String* s1 = new String[count + 1];
    for (int i = 0; i < count + 1; i++)
    {
        s1[i].str = new char[arr[i] + 1];
        s1[i].len = arr[i];
        s1[i].cap = arr[i];
    }
    j = 0;
    k = 0;
    for (int i = 0; i < len; i++)
    {
        if (str[i] != delim)
        {
            s1[j].str[k] = str[i];
            k++;
        }
        else
        {
            s1[j].str[k] = '\0';
            j++;
            k = 0;
        }
    }
    int sum = 0;
    for (int i = 0; i < count + 1; i++)
    {
        sum = sum + arr[i];
    }
    s1[j].str[k] = '\0';
    count++;
    return s1;
}
String* String::tokenize(const char* delim, int& count) const
{
    int s = length(delim);
    count = 0;
    for (int i = 0; i < len; i++)
    {
        bool flag = false;
        for (int j = 0; j < s; j++)
        {
            if (str[i] == delim[j])
            {
                flag = true;
                break;
            }
        }
        if (flag)
        {
            count++;
        }
    }
    int* arr = new int[count + 1];
    int j = 0, k = 0;
    for (int i = 0; i < len; i++)
    {
        bool isDelim = false;
        for (int d = 0; d < s; d++)
        {
            if (str[i] == delim[d])
            {
                isDelim = true;
                break;
            }
        }
        if (!isDelim)
        {
            j++;
        }
        else
        {
            arr[k] = j;
            j = 0;
            k++;
        }
    }
    arr[k] = j;
    String* s1 = new String[count + 1];
    for (int i = 0; i < count + 1; i++)
    {
        s1[i].str = new char[arr[i] + 1];
        s1[i].len = arr[i];
        s1[i].cap = arr[i];
    }
    j = 0;
    k = 0;
    for (int i = 0; i < len; i++)
    {
        bool isDelim = false;
        for (int d = 0; d < s; d++)
        {
            if (str[i] == delim[d])
            {
                isDelim = true;
                break;
            }
        }
        if (!isDelim)
        {
            s1[j].str[k] = str[i];
            k++;
        }
        else
        {
            s1[j].str[k] = '\0';
            j++;
            k = 0;
        }
    }
    s1[j].str[k] = '\0';
    return s1;
}
int* String::search_string(const char* delim, int& count) const
{
    int s = length(delim);
    count = 0;
    for (int i = 0; i < len; i++)
    {
        bool flag = false;
        for (int j = 0; j < s; j++)
        {
            if (str[i] == delim[j])
            {
                flag = true;
                break;
            }
        }
        if (flag)
        {
            count++;
        }
    }
    int* arr = new int[count + 1];
    int j = 0, k = 0;
    for (int i = 0; i < len; i++)
    {
        bool flag = false;
        for (int j = 0; j < s; j++)
        {
            if (str[i] == delim[j])
            {
                flag = true;
                break;
            }
        }
        if (flag)
        {
            arr[j] = i;
            j++;
        }
    }
    return arr;
}
int String::find_first(char ch)
{
    int count = 0;
    for (int i = 0; i < len; i++)
    {
        if (str[i] == ch)
        {
            count = i;
            break;
        }
    }
    return count;
}
int String::find_first(const String& s) const
{
    for (int i = 0; i <= len - s.len; i++)
    {
        int j = 0;
        while (j < s.len && str[i + j] == s.str[j])
        {
            j++;
        }
        if (j == s.len)
        {
            return i;
        }
    }
}
int String::find_last(char ch)
{
    int count = 0;
    for (int i = 0; i < len; i++)
    {
        if (str[i] == ch)
        {
            count = i;
        }
    }
    return count;
}
int String::find_last(const String& s) const
{
    int count = 0;
    for (int i = 0; i <= len - s.len; i++)
    {
        int j = 0;
        while (j < s.len && str[i + j] == s.str[j])
        {
            j++;
        }
        if (j == s.len)
        {
            count = i;
        }
    }
    return count;
}
int* String::find_all(char ch, int& c)
{
    c = 0;
    for (int i = 0; i < len; i++)
    {
        if (str[i] == ch)
        {
            c++;
        }
    }
    int* arr = new int[c + 1];
    int j = 0, k = 0;
    for (int i = 0; i < len; i++)
    {
        if (str[i] != ch)
        {
            j++;
        }
        else
        {
            arr[k] = j;
            j++;
            k++;
        }
    }
    arr[k] = j;
    return arr;
}
int* String::find_all(const String& s, int& c) const
{
    c = 0;
    for (int i = 0; i < len; i++)
    {
        bool flag = false;
        for (int j = 0; j < s.len; j++)
        {
            if (str[i] == s.str[j])
            {
                flag = true;
                break;
            }
        }
        if (flag)
        {
            c++;
        }
    }
    int* arr = new int[c + 1];
    int j = 0, k = 0;
    for (int i = 0; i < len; i++)
    {
        bool flag = false;
        for (int j = 0; j < s.len; j++)
        {
            if (str[i] == s.str[j])
            {
                flag = true;
                break;
            }
        }
        if (flag)
        {
            arr[j] = i;
            j++;
        }
    }
    return arr;
}
void String::remove_at(int i)
{
    if (i > len)
    {
        return;
    }
    for (int j = i; j < len; j++)
    {
        str[j] = str[j + 1];
    }
    len--;
}
void String::insert_at(int i, char ch)
{
    if (i > len)
    {
        return;
    }
    for (int j = len - 1; j >= i; j--)
    {
        str[j + 1] = str[j];
    }
    str[i] = ch;
    len++;
    cap = len;
}
void String::insert_at(int i, const String sub)
{
    for (int j = len - 1; j >= i; j--)
    {
        str[j + sub.len] = str[j];
    }
    for (int j = 0; j < sub.len; j++)
    {
        str[i + j] = sub.str[j];
    }
    len = len + sub.len;
    cap = len;
}
void String::remove_first(char ch)
{
    int idx = 0;
    for (int i = 0; i < len; i++)
    {
        if (str[i] == ch)
        {
            idx = i;
            break;
        }
    }
    remove_at(idx);
}
void String::remove_last(char ch)
{
    int idx = 0;
    for (int i = 0; i < len; i++)
    {
        if (str[i] == ch)
        {
            idx = i;
        }
    }
    remove_at(idx);
}
void String::remove_all(char ch)
{
    for (int i = 0; i < len; i++)
    {
        if (str[i] == ch)
        {
            remove_at(i);
            i--;
        }
    }
}
void String::clear()
{
    delete[] str;
    len = 0;
    cap = 0;
}
String::~String()
{
    delete[] str;
    len = 0;
    cap = 0;
}
void String::upper()
{
    for (int i = 0; i < len; i++)
    {
        if (str[i] >= 'a' && str[i] <= 'z')
        {
            str[i] = str[i] - 32;
        }
    }
}
void String::lower()
{
    for (int i = 0; i < len; i++)
    {
        if (str[i] >= 'A' && str[i] <= 'Z')
        {
            str[i] = str[i] + 32;
        }
    }
}
/*String& operator+(const String& a, const String& b)
{
    String res;
    res.len = a.len + b.len + 1;
    res.cap = res.len + 1;
    res.str = new char[res.len];
    for (int i = 0; i < a.len; i++)
    {
        res.str[i] = a.str[i];
    }
    res.str[a.len] = ' ';
    for (int i = 0; i < b.len; i++)
    {
        res.str[a.len + i + 1] = b.str[i];
    }
    return res;
}*/

