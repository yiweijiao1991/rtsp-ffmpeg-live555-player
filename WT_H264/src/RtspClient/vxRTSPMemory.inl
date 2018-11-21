/*
* Copyright (c) 2014, ����ǧ��
* All rights reserved.
* 
* �ļ����ƣ�vxRTSPMemory.inl
* �������ڣ�2014��7��25��
* �ļ���ʶ��
* �ļ�ժҪ��ʵ�ּ򵥵Ļ����ڴ���У�ר��������ת������-���� ��ϵ�� H264 �������Ƶ֡��
* 
* ��ǰ�汾��1.0.0.0
* ��    �ߣ�
* ������ڣ�2014��7��25��
* �汾ժҪ��
* 
* ȡ���汾��
* ԭ����  ��
* ������ڣ�
* �汾ժҪ��
* 
*/

#include <list>
#include <Windows.h>

///////////////////////////////////////////////////////////////////////////
// x_lock_t

class x_lock_t
{
	// constructor/destructor
public:
	x_lock_t(void)
	{
		InitializeCriticalSection(&m_cs_lock);
	}

	~x_lock_t(void)
	{
#ifdef _DEBUG
		EnterCriticalSection(&m_cs_lock);
		LeaveCriticalSection(&m_cs_lock);
#endif // _DEBUG

		DeleteCriticalSection(&m_cs_lock);
	}

	// public interfaces
public:
	inline void lock(void)
	{
		EnterCriticalSection(&m_cs_lock);
	}

	inline void unlock(void)
	{
		LeaveCriticalSection(&m_cs_lock);
	}

	// class data
protected:
	CRITICAL_SECTION   m_cs_lock;

};

class x_autolock_t
{
	// constructor/destructor
public:
	x_autolock_t(x_lock_t &xt_lock)
		: m_xt_lock(&xt_lock)
	{
		m_xt_lock->lock();
	}

	~x_autolock_t(void)
	{
		m_xt_lock->unlock();
	}

	// class data
private:
	x_lock_t   * m_xt_lock;
};

///////////////////////////////////////////////////////////////////////////
// xmemblock definition

class xmemblock
{
	// class properties
public:
	typedef enum ConstValueID
	{
		BLOCK_INIT_SIZE = 4 * 1024,
	} ConstValueID;

	// constructor/destructor
public:
	xmemblock(size_t st_size = BLOCK_INIT_SIZE);
	~xmemblock(void);

	// public interfaces
public:
	/**************************************************************************
	* Description:
	*     �����ݸ��ӵ��ڴ��β����
	* Parameter:
	*     @[in ] src_buf: ָ��Դ�����ڴ�����
	*     @[in ] st_size: Դ�����ڴ������С�����ֽڼƣ���
	* ReturnValue:
	*     ����ʵ��д�������������ֽڼƣ���
	*     ���� -1����ʾ��������
	*/
	int append_data(const void * src_buf, size_t st_size);

	/**************************************************************************
	* Description:
	*     ������д���ڴ�顣
	* Parameter:
	*     @[in ] src_buf: ָ��Դ�����ڴ�����
	*     @[in ] st_size: Դ�����ڴ������С�����ֽڼƣ���
	* ReturnValue:
	*     ����ʵ��д�������������ֽڼƣ���
	*     ���� -1����ʾ��������
	*/
	int write_block(const void * src_buf, size_t st_size);

	/**************************************************************************
	* Description:
	*     ���ڴ�����ݶ�ȡ��Ŀ���ڴ�����
	* Parameter:
	*     @[out] dst_buf: Ŀ���ڴ�����
	*     @[in ] st_size: Ŀ���ڴ������С��
	* ReturnValue:
	*     ����ʵ�ʶ�ȡ�����������ֽڼƣ���
	*     ���� -1����ʾ��������
	*/
	int read_block(void *dst_buf, size_t st_size);

	/**************************************************************************
	* Description:
	*     �����ڴ��Ŀռ��С������д�ռ��С��ע�⣬�ò���������ԭ�ڴ���е����ݣ���
	* Parameter:
	*     @[in ] st_max_size: �ڴ��Ŀռ��С��
	* ReturnValue:
	*     �����·�����ڴ��ռ��С��
	*     ���� -1����ʾ��������
	*/
	int set_max_size(size_t st_max_size);

	/**************************************************************************
	* Description:
	*     �Զ������ڴ��Ŀռ��Сֵ��
	*     ������ֵδ�����ڴ��ռ�����ֵ�������·����ڴ�����
	*     ������ֵ�����ڴ��ռ�����ֵ������� set_max_size() �����ڴ�ֵ��
	* Parameter:
	*     @[in ] st_auto_resize: �Զ�����ֵ��
	* ReturnValue:
	*     �����ڴ��Ŀռ��С������д�ռ䣩��
	*/
	int auto_resize(size_t st_auto_resize);

	/**************************************************************************
	* Description:
	*     ����ָ���ڴ������ݵ�ַ��
	*/
	inline const void * data(void)
	{
		return m_data_ptr;
	}

	/**************************************************************************
	* Description:
	*     �����ڴ�鵱ǰʹ�õ������������С�����ã������ݿ���ʼλ�õ�ƫ��������
	*/
	inline size_t& size(void)
	{
		return m_st_len;
	}

	/**************************************************************************
	* Description:
	*     ��д�����ݵĳ��������㡣
	*/
	inline void reset(void)
	{
		m_st_len = 0;
	}

	/**************************************************************************
	* Description:
	*     �����ڴ��Ŀռ��С������д�ռ䣩��
	*/
	inline size_t max_size(void) const
	{
		return m_st_max;
	}

	// class data
protected:
	unsigned char * m_data_ptr;
	size_t          m_st_max;
	size_t          m_st_len;
};

///////////////////////////////////////////////////////////////////////////
// xmemblock implementation

//=========================================================================

// 
// xmemblock constructor/destructor
// 

xmemblock::xmemblock(size_t st_size /* = BLOCK_INIT_SIZE */)
{
	m_data_ptr = (unsigned char *)calloc(st_size, sizeof(char));
	m_st_len = 0;
	m_st_max = st_size;
}

xmemblock::~xmemblock(void)
{
	if (NULL != m_data_ptr)
	{
		free(m_data_ptr);
	}
}

//=========================================================================

// 
// xmemblock public interfaces
// 

/**************************************************************************
* Description:
*     �����ݸ��ӵ��ڴ��β����
* Parameter:
*     @[in ] src_buf: ָ��Դ�����ڴ�����
*     @[in ] st_size: Դ�����ڴ������С�����ֽڼƣ���
* ReturnValue:
*      ����ʵ��д�������������ֽڼƣ���
*      ���� -1����ʾ��������
*/
int xmemblock::append_data(const void *src_buf, size_t st_size)
{
	if ((NULL == src_buf) || (0 == st_size))
		return 0;

	size_t new_len = st_size + size();

	if (new_len > m_st_max)
	{
		unsigned char * _data_new_ptr = (unsigned char *)((NULL != m_data_ptr) ?
			realloc(m_data_ptr, new_len) : calloc(new_len, sizeof(char)));

		if (NULL == _data_new_ptr)
			return -1;

		m_data_ptr = _data_new_ptr;
		m_st_max = new_len;
	}

	memcpy(m_data_ptr + size(), src_buf, st_size);
	m_st_len = new_len;

	return (int)m_st_len;
}

/**************************************************************************
* Description:
*     ������д���ڴ�顣
* Parameter:
*     @[in ] src_buf: ָ��Դ�����ڴ�����
*     @[in ] st_size: Դ�����ڴ������С�����ֽڼƣ���
* ReturnValue:
*      ����ʵ��д�������������ֽڼƣ���
*      ���� -1����ʾ��������
*/
int xmemblock::write_block(const void *src_buf, size_t st_size)
{
	if ((NULL == src_buf) || (0 == st_size))
		return 0;

	if (st_size > m_st_max)
	{
		unsigned char * _data_new_ptr = (unsigned char *)malloc(st_size);
		if (NULL == _data_new_ptr)
			return -1;

		if (NULL != m_data_ptr)
			free(m_data_ptr);
		m_data_ptr = _data_new_ptr;
		m_st_max = st_size;
	}

	memcpy(m_data_ptr, src_buf, st_size);
	m_st_len = st_size;

	return (int)m_st_len;
}

/**************************************************************************
* Description:
*     ���ڴ�����ݶ�ȡ��Ŀ���ڴ�����
* Parameter:
*     @[out] dst_buf: Ŀ���ڴ�����
*     @[in ] st_size: Ŀ���ڴ������С��
* ReturnValue:
*      ����ʵ�ʶ�ȡ�����������ֽڼƣ���
*      ���� -1����ʾ��������
*/
int xmemblock::read_block(void *dst_buf, size_t st_size)
{
	if ((NULL == dst_buf) || (0 == st_size))
		return 0;

	int read_bytes = (st_size < m_st_len) ? st_size : m_st_len;
	memcpy(dst_buf, m_data_ptr, read_bytes);

	return read_bytes;
}

/**************************************************************************
* Description:
*     �����ڴ��Ŀռ��С������д�ռ��С��ע�⣬�ò���������ԭ�ڴ���е����ݣ���
* Parameter:
*     @[in] st_max_size: �ڴ��Ŀռ��С��
* ReturnValue:
*      �����·�����ڴ��ռ��С��
*      ���� -1����ʾ��������
*/
int xmemblock::set_max_size(size_t st_max_size)
{
	unsigned char * _data_new_ptr = (unsigned char *)malloc(st_max_size);
	if (NULL == _data_new_ptr)
		return -1;

	if (NULL != m_data_ptr)
		free(m_data_ptr);
	m_data_ptr = _data_new_ptr;
	m_st_max = st_max_size;

	return m_st_max;
}

/**************************************************************************
* Description:
*     �Զ������ڴ��Ŀռ��Сֵ��
*     ������ֵδ�����ڴ��ռ�����ֵ�������·����ڴ�����
*     ������ֵ�����ڴ��ռ�����ֵ������� set_max_size() �����ڴ�ֵ��
* Parameter:
*     @[in ] st_auto_resize: �Զ�����ֵ��
* ReturnValue:
*     �����ڴ��Ŀռ��С������д�ռ䣩��
*/
int xmemblock::auto_resize(size_t st_auto_resize)
{
	if (st_auto_resize > max_size())
	{
		set_max_size(st_auto_resize);
	}
	return max_size();
}

///////////////////////////////////////////////////////////////////////////
// xmemblock_cirqueue definition

class xmemblock_cirqueue
{
	// class property
public:
	typedef enum ConstValueID
	{
		QUEUE_INIT_MAX_BLOCKS = 500,
	} ConstValueID;

	// constructor/destructor
public:
	xmemblock_cirqueue(size_t st_max_blocks = QUEUE_INIT_MAX_BLOCKS);
	~xmemblock_cirqueue(void);

	// public interfaces
public:
	/**************************************************************************
	* Description:
	*     �����ڴ���������
	*     ע��:�����������������жϿ��ж������Ƿ�Ϊ�գ�����Ϊ�գ���ȡ���ж����еĶ���
	*          ��Ϊ������󷵻أ���Ϊ�գ����жϴ洢�����Ƿ�ﵽ���洢���������ﵽ��
	*          ��ȡ�洢���ж��˶�����Ϊ������󷵻أ������´������󷵻ء�
	* Parameter:
	*     
	* ReturnValue:
	*     �ɹ�������������ڴ�����
	*     ʧ�ܣ����� NULL��
	*/
	xmemblock * alloc(void);

	/**************************************************************************
	* Description:
	*     �����ڴ�����
	* Parameter:
	*     @[in ] _block_ptr: Ҫ���յ��ڴ�����
	* ReturnValue:
	*     void
	*/
	void recyc(xmemblock *_block_ptr);

	/**************************************************************************
	* Description:
	*     ����Ҫ�������ݵ��ڴ�����ѹ�뻷���ڴ����еĴ洢���е׶ˡ�
	* Parameter:
	*     @[in ] _block_ptr: Ҫ�������ݵ��ڴ�����
	* ReturnValue:
	*     void
	*/
	void push_back_to_saved_queue(xmemblock *_block_ptr);

	/**************************************************************************
	* Description:
	*     �Ӵ洢������ȡ�����˵��ڴ�����Ȼ��ͨ���ö��������Ӧ�����ݡ�
	* ReturnValue:
	*     �������类д�����ݵ��ڴ�����
	*     ������ NULL ����ʾ�洢����Ϊ�ա�
	*/
	xmemblock * pop_front_from_saved_queue(void);

	/**************************************************************************
	* FunctionName:
	*     cleanup_saved_queue
	* Description:
	*     ����������ѱ�����ڴ����󣨻��������ж����У���
	*/
	void cleanup_saved_queue(void);

	/**************************************************************************
	* Description:
	*     ������д��洢�ڴ������С�
	* Parameter:
	*     @[in ] src_buf: Դ���ݡ�
	*     @[in ] st_len: Դ���ݳ��ȡ�
	* ReturnValue:
	*     ʵ��д�����ݵĳ��ȣ����ֽڼƣ���
	*     ���� -1����ʾ��������
	*/
	int write_in(const void * src_buf, size_t st_len);

	/**************************************************************************
	* Description:
	*     �Ӵ洢������ȡ�����˵��ڴ���������ݡ�
	* Parameter:
	*     @[out] dst_buf: �������ݵ�Ŀ���ڴ滺������
	*     @[in ] st_len: Ŀ���ڴ滺������С��
	* ReturnValue:
	*     ʵ�ʶ�ȡ���ݵĳ��ȣ����ֽڼƣ���
	*     ���� -1����ʾ��������
	*/
	int read_out(void * dst_buf, size_t st_len);

	/**************************************************************************
	* Description:
	*     ���ػ����ڴ����е��������ڴ�����������
	*/
	inline size_t max_blocks_size(void) const
	{
		return m_st_max_blocks;
	}

	/**************************************************************************
	* Description:
	*     ���û����ڴ����е��������ڴ�����������
	* Parameter:
	*     @[in ] st_max_blocks: �������ڴ�����������
	* ReturnValue:
	*     ��������ǰ �������ڴ�����������
	*/
	size_t resize_max_blocks(size_t st_max_blocks);

	/**************************************************************************
	* Description:
	*     �ͷŻ����ڴ����������е��ڴ�����
	*/
	void clear_cir_queue(void);

	// class data
protected:
	x_lock_t                   m_lock_lst_free;     // ���е��ڴ�����Ķ��з�����
	std::list< xmemblock * >   m_lst_free;          // ���е��ڴ�����
	x_lock_t                   m_lock_lst_save;     // �洢���ڴ�����Ķ��з�����
	std::list< xmemblock * >   m_lst_save;          // �洢���ڴ�����
	volatile size_t            m_st_max_blocks;     // �������ڴ���������

};

///////////////////////////////////////////////////////////////////////////
// 

//=========================================================================

// 
// constructor/destructor
// 

xmemblock_cirqueue::xmemblock_cirqueue(size_t st_max_blocks /* = PAGE_INIT_MAX_BLOCKS */)
		: m_st_max_blocks(st_max_blocks)
{

}

xmemblock_cirqueue::~xmemblock_cirqueue(void)
{
	clear_cir_queue();
}

//=========================================================================

// 
// public interfaces
// 

/**************************************************************************
* Description:
*     �����ڴ���������
*     ע��:�����������������жϿ��ж������Ƿ�Ϊ�գ�����Ϊ�գ���ȡ���ж����еĶ���
*          ��Ϊ������󷵻أ���Ϊ�գ����жϴ洢�����Ƿ�ﵽ���洢���������ﵽ��
*          ��ȡ�洢���ж��˶�����Ϊ������󷵻أ������´������󷵻ء�
* Parameter:
*     
* ReturnValue:
*      �ɹ�������������ڴ�����
*      ʧ�ܣ����� NULL��
*/
xmemblock * xmemblock_cirqueue::alloc(void)
{
	xmemblock * _block_ptr = NULL;

	x_autolock_t auto_lock_free(m_lock_lst_free);
	if (m_lst_free.empty())
	{
		x_autolock_t auto_lock_save(m_lock_lst_save);
		if (m_lst_save.size() < max_blocks_size())
			_block_ptr = new xmemblock((size_t)xmemblock::BLOCK_INIT_SIZE);
		else
		{
			_block_ptr = m_lst_save.front();
			m_lst_save.pop_front();
		}
	}
	else
	{
		_block_ptr = m_lst_free.front();
		m_lst_free.pop_front();
	}

	return _block_ptr;
}

/**************************************************************************
* Description:
*     �����ڴ�����
* Parameter:
*     @[in ] _block_ptr: Ҫ���յ��ڴ�����
* ReturnValue:
*     void
*/
void xmemblock_cirqueue::recyc(xmemblock * _block_ptr)
{
	if (NULL == _block_ptr)
		return;

	x_autolock_t auto_lock_free(m_lock_lst_free);
	if (m_lst_free.size() < max_blocks_size())
		m_lst_free.push_back(_block_ptr);
	else
		delete _block_ptr;
}

/**************************************************************************
* Description:
*     ����Ҫ�������ݵ��ڴ�����ѹ�뻷���ڴ����еĴ洢���е׶ˡ�
* Parameter:
*     @[in ] _block_ptr: Ҫ�������ݵ��ڴ�����
* ReturnValue:
*     void
*/
void xmemblock_cirqueue::push_back_to_saved_queue(xmemblock *_block_ptr)
{
	if (NULL == _block_ptr)
		return;

	x_autolock_t auto_lock_save(m_lock_lst_save);
	m_lst_save.push_back(_block_ptr);
}

/**************************************************************************
* Description:
*     �Ӵ洢������ȡ�����˵��ڴ�����Ȼ��ͨ���ö��������Ӧ�����ݡ�
* ReturnValue:
*     �������类д�����ݵ��ڴ�����
*     ������ NULL ����ʾ�洢����Ϊ�ա�
*/
xmemblock * xmemblock_cirqueue::pop_front_from_saved_queue(void)
{
	x_autolock_t auto_lock_save(m_lock_lst_save);

	xmemblock *_block_ptr = NULL;
	if (!m_lst_save.empty())
	{
		_block_ptr = m_lst_save.front();
		m_lst_save.pop_front();
	}

	return _block_ptr;
}

/**************************************************************************
* FunctionName:
*     cleanup_saved_queue
* Description:
*     ����������ѱ�����ڴ����󣨻��������ж����У���
*/
void xmemblock_cirqueue::cleanup_saved_queue(void)
{
	x_autolock_t auto_lock_save(m_lock_lst_save);
	x_autolock_t auto_lock_free(m_lock_lst_free);

	std::list< xmemblock * >::iterator itlst = m_lst_save.begin();
	for (; itlst != m_lst_save.end(); ++itlst)
	{
		xmemblock * _block_ptr = *itlst;
		if (m_lst_free.size() < max_blocks_size())
			m_lst_free.push_back(_block_ptr);
		else
			delete _block_ptr;
	}

	m_lst_save.clear();
}

/**************************************************************************
* Description:
*     ������д��洢�ڴ������С�
* Parameter:
*     @[in ] src_buf: Դ���ݡ�
*     @[in ] st_len: Դ���ݳ��ȡ�
* ReturnValue:
*     ʵ��д�����ݵĳ��ȣ����ֽڼƣ���
*     ���� -1����ʾ��������
*/
int xmemblock_cirqueue::write_in(const void * src_buf, size_t st_len)
{
	if ((NULL == src_buf) || (0 == st_len))
		return 0;

	xmemblock * _block_ptr = alloc();
	if (NULL == _block_ptr)
		return 0;

	int i_write = _block_ptr->write_block(src_buf, st_len);
	if (i_write > 0)
		push_back_to_saved_queue(_block_ptr);
	else
		recyc(_block_ptr);

	return i_write;
}

/**************************************************************************
* Description:
*     �Ӵ洢������ȡ�����˵��ڴ���������ݡ�
* Parameter:
*     @[out] dst_buf: �������ݵ�Ŀ���ڴ滺������
*     @[in ] st_len: Ŀ���ڴ滺������С��
* ReturnValue:
*     ʵ�ʶ�ȡ���ݵĳ��ȣ����ֽڼƣ���
*     ���� -1����ʾ��������
*/
int xmemblock_cirqueue::read_out(void * dst_buf, size_t st_len)
{
	if ((NULL == dst_buf) || (0 == st_len))
		return 0;

	xmemblock * _block_ptr = pop_front_from_saved_queue();
	if ((NULL == _block_ptr) || (0 == _block_ptr->size()))
		return 0;
	int i_read = _block_ptr->read_block(dst_buf, st_len);
	recyc(_block_ptr);

	return i_read;
}

/**************************************************************************
* Description:
*     ���û����ڴ����е��������ڴ�����������
* Parameter:
*     @[in ] st_max_blocks: �������ڴ�����������
* ReturnValue:
*     ��������ǰ �������ڴ�����������
*/
size_t xmemblock_cirqueue::resize_max_blocks(size_t st_max_blocks)
{
	x_autolock_t auto_lock(m_lock_lst_free);
	while (m_lst_free.size() > st_max_blocks)
	{
		xmemblock * _block_ptr = m_lst_free.back();
		m_lst_free.pop_back();
		delete _block_ptr;
	}

	return (size_t)InterlockedExchange((LONG *)&m_st_max_blocks, (LONG)st_max_blocks);
}

/**************************************************************************
* Description:
*     �ͷŻ����ڴ����������е��ڴ�����
*/
void xmemblock_cirqueue::clear_cir_queue(void)
{
	x_autolock_t auto_lock_free(m_lock_lst_free);
	std::list< xmemblock * >::iterator itls_free = m_lst_free.begin();
	for (; itls_free != m_lst_free.end(); ++itls_free)
	{
		delete *itls_free;
	}
	m_lst_free.clear();

	x_autolock_t auto_lock_save(m_lock_lst_save);
	std::list< xmemblock * >::iterator itls_used = m_lst_save.begin();
	for (; itls_used != m_lst_save.end(); ++itls_used)
	{
		delete *itls_used;
	}
	m_lst_save.clear();
}

