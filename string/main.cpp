#include <cstring>
#include <stdexcept>

class String
{
public:
    String()
    {
        m_c_str = nullptr;
        m_len = 0;
        m_capacity = 0;
    }
    
    ~String()
    {
        delete[] m_c_str;
    }
    
    String(const String& str)
    {
        m_capacity = str.m_capacity;
        m_len = str.m_len;
        m_c_str = new char[m_capacity];
        strcpy(m_c_str, str.m_c_str);
    }

    String(const char* c_str)
    {
        m_len = strlen(c_str);
        m_capacity = m_len + 1;
        m_c_str = new char[m_capacity];
        strcpy(m_c_str, c_str);
    }

    String(String&& str) noexcept
    {
        m_capacity = str.m_capacity;
        m_len = str.m_len;
        m_c_str = str.m_c_str;
        str.m_c_str = nullptr;
        str.m_len = 0;
        str.m_capacity = 0;
    }

    String& operator=(const String& str)
    {
        if (this == &str)
        {
            return *this;
        }
        *this = str.m_c_str;
        return *this;
    }

    String& operator=(String&& str) noexcept
    {
        if (this == &str)
        {
            return *this;
        }
        delete[] m_c_str;
        m_capacity = str.m_capacity;
        m_len = str.m_len;
        m_c_str = str.m_c_str;
        str.m_c_str = nullptr;
        str.m_len = 0;
        str.m_capacity = 0;
        return *this;
    }

    String& operator=(const char* c_str)
    {
        delete[] m_c_str;
        m_len = strlen(c_str);
        m_capacity = m_len + 1;
        m_c_str = new char[m_capacity];
        strcpy(m_c_str, c_str);
        return *this;
    }

    void push_back(char c)
    {
        if (m_len + 1 == m_capacity)
        {
            size_t new_capacity = (m_capacity == 0 ? 1 : m_capacity * 2);
            expandCapacity(new_capacity);
        }
        m_c_str[m_len++] = c;
        m_c_str[m_len] = 0;
    }

    void push_back(const String& str)
    {
        push_back(str.m_c_str);
    }

    void push_back(const char* c_str)
    {
        size_t new_len = strlen(c_str) + m_len;
        if (new_len + 1 >= m_capacity)
        {
            size_t new_capacity = m_capacity;
            if (new_capacity == 0)
            {
                new_capacity = new_len + 1;
            }
            else
            {
                while (new_len + 1 > new_capacity)
                {
                    new_capacity *= 2;
                }
            }
            expandCapacity(new_capacity);
            m_capacity = new_capacity;
        }
        strcpy(m_c_str + m_len, c_str);
        m_len = new_len;
    }

    [[nodiscard]] size_t length() const
    {
        return m_len;
    }

    [[nodiscard]] bool empty() const
    {
        return m_len == 0;
    }

    char& operator[](size_t i)
    {
        if (i < m_len)
        {
            return m_c_str[i];
        }
        throw std::out_of_range("invalid index.");
    }

    const char& operator[](size_t i) const
    {
        if (i < m_len)
        {
            return m_c_str[i];
        }
        throw std::out_of_range("invalid index.");
    }

private:
    void expandCapacity(size_t capacity)
    {
        char* new_c_str = new char[capacity];
        if (m_c_str)
        {
            strcpy(new_c_str, m_c_str);
            delete[] m_c_str;
        }
        m_c_str = new_c_str;
        m_capacity = capacity;
    }

    char* m_c_str;
    size_t m_len;
    size_t m_capacity;
};

int main()
{
    String str = "abcasdkjaldfhjlkasdjflasdkf";
    str.push_back("11111jadhkahjsdfgkjahsdkjhaksdfh");
    str.push_back('9');
    String str2(str);
    String str3 = str2;
    String str4 = std::move(str3);
    String str5(std::move(str4));
    return 0;
}