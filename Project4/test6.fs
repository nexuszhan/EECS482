/ (type d) (inode block 0)
	owner: 
	size: 1
	data disk blocks: 1 
	entry 0: dir, inode block 2
	entry 1: dir1, inode block 3

/dir (type d) (inode block 2)
	owner: user1
	size: 1
	data disk blocks: 4 
	entry 0: file, inode block 5
	entry 1: dir, inode block 10

/dir/file (type f) (inode block 5)
	owner: user1
	size: 1
	data disk blocks: 8 
We hold these truths to be self-evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness. -- That to secure these rights, Governments are instituted among Men, deriving their just powers from the consent of the governed, -- That whenever any Form of Government becomes destructive of these ends, it is the Right of the People to alter or to abolish it, and to institute new Government, laying

/dir/dir (type d) (inode block 10)
	owner: user1
	size: 1
	data disk blocks: 11 
	entry 0: dir, inode block 12

/dir/dir/dir (type d) (inode block 12)
	owner: user1
	size: 1
	data disk blocks: 13 
	entry 0: file, inode block 14

/dir/dir/dir/file (type f) (inode block 14)
	owner: user1
	size: 0
	data disk blocks: 


/dir1 (type d) (inode block 3)
	owner: user2
	size: 1
	data disk blocks: 6 
	entry 0: file, inode block 7

/dir1/file (type f) (inode block 7)
	owner: user2
	size: 1
	data disk blocks: 9 
We hold these truths to be self-evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness. -- That to secure these rights, Governments are instituted among Men, deriving their just powers from the consent of the governed, -- That whenever any Form of Government becomes destructive of these ends, it is the Right of the People to alter or to abolish it, and to institute new Government, laying

4081 disk blocks free
