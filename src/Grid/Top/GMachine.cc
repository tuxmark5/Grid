#include <Grid/App/G5App.hh>
#include <Grid/Bottom/G1Physical.hh>
#include <Grid/Bottom/G2DataLink.hh>
#include <Grid/Bottom/G3Network.hh>
#include <Grid/Bottom/G4Transport.hh>
#include <Grid/Bottom/GProcess.hh>
#include <Grid/Top/GInterface.hh>
#include <Grid/Top/GMachine.hh>
#include <Grid/Top/GSocket.hh>

/********************************************* TX *********************************************/
/*                                          GMachine                                          */
/**********************************************************************************************/

Vacuum GMachine :: GMachine(const QByteArray& name):
  m_name(name),
  m_destroying(false)
{
  m_core    = new GFiber(std::bind(&GMachine::run, this));
  m_layer4  = new G4Transport(this);
}

/**********************************************************************************************/

Vacuum GMachine :: GMachine(const GMachine& other)
{
}

/**********************************************************************************************/

Vacuum GMachine :: ~GMachine()
{
  m_destroying = true;
  gDeleteAll(m_processes);
  delete m_layer4;
}

/**********************************************************************************************/

Void GMachine :: printRoutingTables()
{
  //for (auto )
}

/**********************************************************************************************/

Void GMachine :: removeProcess(GProcess* process)
{
  G_GUARD(!m_destroying, Vacuum);

  layer4()->removeProcess(process);
  m_processes.remove(process->pid());
}

/**********************************************************************************************/

Void GMachine :: run()
{
  while (true)
  {
    while (!m_oqueue.empty())
    {
      m_oqueue.first().processOutput();
      m_oqueue.removeFirst();
      GFiber::yield();
    }

    m_omon.wait();
  }
}

/**********************************************************************************************/

Void GMachine :: send(GFrame& frame)
{
  if (m_oqueue.size() > 30)
  {
    printf("[[OUTPUT QUEUE TOO LARGE]]\n");
    exit(0);
  }
  m_oqueue.append(frame);
  m_omon.notify();
}

/**********************************************************************************************/

GInterface GMachine :: operator () (U8 mac, U4 address, U1 length)
{
  return GInterface(this, mac, address, length);
}

/**********************************************************************************************/

GMachine& GMachine :: operator << (std::function<void()> process)
{
  while (true)
  {
    int pid = qrand() % 0x10000;

    if (!m_processes.contains(pid))
    {
      m_processes.insert(pid, new GProcess(this, pid, std::move(process)));
      break;
    }
  }

  return *this;
}

/**********************************************************************************************/

GMachine& GMachine :: operator << (G5App* process)
{
  process->m_machine = this;

  return *this << [=]()
  {
    process->run();
    delete process;
  };
}

/**********************************************************************************************/

GMachine& machine(const QByteArray& name)
{
  return *(new GMachine(name));
}

/**********************************************************************************************/
