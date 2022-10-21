// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "loongarch.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

typedef struct {
  struct spinlock lock;
  struct buf buf[5];
  struct buf head;
} bucket;

bucket buckets[13];

void
binit(void)
{
  struct buf *b;
  int i;
  for (i = 0; i < 13; ++i) {
    initlock(&buckets[i].lock, "bcache");
  // Create linked list of buffers
    buckets[i].head.prev = &buckets[i].head;
    buckets[i].head.next = &buckets[i].head;
    for(b = buckets[i].buf; b < buckets[i].buf+5; b++){
      b->next = buckets[i].head.next;
      b->prev = &buckets[i].head;
      initsleeplock(&b->lock, "buffer");
      buckets[i].head.next->prev = b;
      buckets[i].head.next = b;
    }
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;
  int id = blockno % 13;

  acquire(&buckets[id].lock);

  // Is the block already cached?
  for(b = buckets[id].head.next; b != &buckets[id].head; b = b->next){
    if(b->dev == dev && b->blockno == blockno){
      b->refcnt++;
      release(&buckets[id].lock);
      acquiresleep(&b->lock);
      return b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  for(b = buckets[id].head.prev; b != &buckets[id].head; b = b->prev){
    if(b->refcnt == 0) {
      b->dev = dev;
      b->blockno = blockno;
      b->valid = 0;
      b->refcnt = 1;
      release(&buckets[id].lock);
      acquiresleep(&b->lock);
      return b;
    }
  }
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    ramdiskrw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  ramdiskrw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);
  int id = b->blockno % 13;

  acquire(&buckets[id].lock);
  b->refcnt--;
  if (b->refcnt == 0) {
    // no one is waiting for it.
    b->next->prev = b->prev;
    b->prev->next = b->next;
    b->next = buckets[id].head.next;
    b->prev = &buckets[id].head;
    buckets[id].head.next->prev = b;
    buckets[id].head.next = b;
  }
  
  release(&buckets[id].lock);
}

void
bpin(struct buf *b) {
  int id = b->blockno % 13;
  acquire(&buckets[id].lock);
  b->refcnt++;
  release(&buckets[id].lock);
}

void
bunpin(struct buf *b) {
  int id = b->blockno % 13;
  acquire(&buckets[id].lock);
  b->refcnt--;
  release(&buckets[id].lock);
}

