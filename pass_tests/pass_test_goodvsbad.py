import chipwhisperer
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
from matplotlib.pylab import *

cwapi = CWCoreAPI()


#cwapi.openProject(r'c:\users\colin\chipwhisperer_projects\tmp\mpc5748g_password_group0_100k.cwp')
#knownkey = "0000000000000000FFFFFFFFFFFFFFFF00000000FFFFFFFF0000000000000000"


cwapi.openProject(r'c:\users\colin\chipwhisperer_projects\tmp\mpc5748g_password_group3_100k.cwp')
knownkey = "5c3c3dc267b1d8f792f633c51389356c104100f0b52f1aa7f85c2c786d376cf8"
#knownkey = "6d376cf8f85c2c78b52f1aa7104100f01389356c92f633c567b1d8f75c3c3dc2"


knownkey = [int(knownkey[i:(i+2)], 16) for i in range(0, 64, 2)]

#HD Tests
knownkey[4:] = [knownkey[i+0] ^ knownkey[i+4] for i in range(0, 32-4)]


tm = cwapi.project().traceManager()


#ppMod0 = chipwhisperer.analyzer.preprocessing.resync_sad.ResyncSAD(cwapi.project().traceManager(), connectTracePlot=False)
#ppMod0.setEnabled(True)
##ppMod0.setReference(rtraceno=0, refpoints=(12,211), inputwindow=(0,487))
#ppMod0.setReference(rtraceno=0, refpoints=(513,657), inputwindow=(326,713))
#ppMod0.init()
#
#ppMod4 = chipwhisperer.analyzer.preprocessing.cache_traces.CacheTraces(ppMod0)
#ppMod4.setEnabled(False)
#ppMod4.init()

#tm = ppMod4

hold(True)

ones = np.zeros(tm.numPoints())
zeros = np.zeros(tm.numPoints())
numones = 0
numzeros = 0

for i in range(0, tm.numTraces()):
    ct = tm.getTextout(i)
    pt = tm.getTextin(i)
    
    if i > 1000:
        ones += tm.getTrace(i)
        numones += 1
    else:
        zeros += tm.getTrace(i)
        numzeros += 1
        

ones = ones / numones
zeros = zeros / numzeros
    
plot(ones - zeros, 'b')
    