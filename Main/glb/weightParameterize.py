from loadAnimation import *
import math

class WeightParameterize:
    def __init__(self, bid, bw, min_bw=0.0001):
        #assemble to log space
        log_bw = []
        off_bw = []
        off = 0
        for i in range(bw.shape[0]):
            off_bwi = []
            for d in range(4):
                if bid[i,d]>=0:
                    log_bw.append(math.log(max(min_bw,bw[i,d])))
                    off_bwi.append(off)
                    off += 1
                else: off_bwi.append(-1)
            off_bw.append(off_bwi)
        self.log_bw = torch.tensor(log_bw).cuda()
        
        #spread matrix
        row_offset = [0]
        col_index = []
        value = []
        for i in range(bw.shape[0]):
            for d in range(4):
                if bid[i,d]>=0:
                    col_index.append(off_bw[i][d])
                    value.append(1.)
                row_offset.append(len(col_index))
        self.spread = torch.sparse_csr_tensor(torch.tensor(row_offset), torch.tensor(col_index), torch.tensor(value), dtype=torch.float32).cuda()
        
        #sum matrix 
        row_offset = [0]
        col_index = []
        value = []
        for i in range(bw.shape[0]):
            for d in range(4):
                if bid[i,d]>=0:
                    col_index.append(off_bw[i][d])
                    value.append(1.)
            row_offset.append(len(col_index))
        self.sum = torch.sparse_csr_tensor(torch.tensor(row_offset), torch.tensor(col_index), torch.tensor(value), dtype=torch.float32).cuda()
        
    def assemble_bw(self):
        exp_bw_compact = torch.exp(self.log_bw)
        exp_bw = torch.sparse.mm(self.spread, exp_bw_compact.unsqueeze(1))
        sum_exp_bw = torch.sparse.mm(self.sum, exp_bw_compact.unsqueeze(1))
        return exp_bw.reshape((-1, 4)) / sum_exp_bw.expand((sum_exp_bw.shape[0], 4))
    
if __name__=='__main__':
    bid = torch.tensor([[0,-1,-1,-1],[1,2,-1,-1],[3,1,2,-1]],dtype=torch.int32).cuda()
    bw = torch.tensor([[1,0,0,0],[.25,.75,0,0],[.1,.2,.7,0]],dtype=torch.float32).cuda()
    wp = WeightParameterize(bid,bw)
    print('bw-input=%s'%str(bw))
    print('bw-recon=%s'%str(wp.assemble_bw()))