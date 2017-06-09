import chipwhisperer
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
from matplotlib.pylab import *

cwapi = CWCoreAPI()


#cwapi.openProject(r'c:\users\colin\chipwhisperer_projects\tmp\mpc5748g_password_group0_100k.cwp')
#knownkey = "0000000000000000FFFFFFFFFFFFFFFF00000000FFFFFFFF0000000000000000"


#cwapi.openProject(r'c:\users\colin\chipwhisperer_projects\tmp\mpc5748g_password_group3_100k.cwp')
cwapi.openProject(r'e:\mpcfiles\mpc5748g_pw_1core_200k_randtext_group3.cwp')
#Knownkey for group 3 as sent
knownkey = "5c3c3dc267b1d8f792f633c51389356c104100f0b52f1aa7f85c2c786d376cf8"
#Knownkey for group 3 as written in FLASH (which is bakcwards to how comparison actually ahppens)
#knownkey = "6d376cf8f85c2c78b52f1aa7104100f01389356c92f633c567b1d8f75c3c3dc2"


knownkey = [int(knownkey[i:(i+2)], 16) for i in range(0, 64, 2)]

#HD Tests
knownkey[4:] = [knownkey[i-4] ^ knownkey[i] for i in range(0, 32-4)]


tm = cwapi.project().traceManager()


#ppMod0 = chipwhisperer.analyzer.preprocessing.resync_sad.ResyncSAD(cwapi.project().traceManager(), connectTracePlot=False)
#ppMod0.setEnabled(True)
##ppMod0.setReference(rtraceno=0, refpoints=(12,211), inputwindow=(0,487))
#ppMod0.setReference(rtraceno=0, refpoints=(513,657), inputwindow=(326,713))
#ppMod0.init()

#ppMod4 = chipwhisperer.analyzer.preprocessing.cache_traces.CacheTraces(ppMod0)
#pMod4.setEnabled(True)
#ppMod4.init()

#tm = ppMod4

hold(True)

avgbyte = False

a = None


for byte in [4,5,6,7]:#[0,1,4,8,9,12]:
    
    if avgbyte:
        ones = np.zeros(tm.numPoints())
        zeros = np.zeros(tm.numPoints())
        numones = 0
        numzeros = 0   

    for bit in range(0,8):
        
        if not avgbyte:
            ones = np.zeros(tm.numPoints())
            zeros = np.zeros(tm.numPoints())
            numones = 0
            numzeros = 0           
        
        print "%d-%d"%(byte,bit)
        mask = 1<<bit       
        for i in range(0, tm.numTraces()):
            ct = tm.getTextout(i)
            pt = tm.getTextin(i)
            
            if pt[byte] & mask:
                ones += tm.getTrace(i)
                numones += 1
            else:
                zeros += tm.getTrace(i)
                numzeros += 1
                
        if not avgbyte:
            ones = ones / numones
            zeros = zeros / numzeros
                
            if knownkey[byte] & mask:
                plot(ones - zeros, 'r')
            else:
                plot(ones - zeros, 'b')
                
            #if knownkey[byte] & mask:
            #    test = where((ones-zeros) > 0.0, (ones-zeros), 0.0)
            #else:
            #    test = where((ones-zeros) < 0.0, (ones-zeros), 0.0)                
               
            #if a is None:
            #    a = test
            #else:
            #    a = a * test * 10000
                               
        
    if avgbyte:
        ones = ones / numones
        zeros = zeros / numzeros
            
        if knownkey[byte] & mask:
            plot(ones - zeros, 'r')
        else:
            plot(ones - zeros, 'b')

    
#plot(a)
#print where(a > 0.0)[0]
