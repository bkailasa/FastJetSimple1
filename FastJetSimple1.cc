// -*- C++ -*-
//
// Package:    FastjetEx/FastJetSimple1
// Class:      FastJetSimple1
//
/**\class FastJetSimple1 FastJetSimple1.cc FastjetEx/FastJetSimple1/plugins/FastJetSimple1.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Balashangar Kailasapathy
//         Created:  Sat, 07 Nov 2020 01:13:32 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

//======Additional header files==================
#include "FWCore/Framework/interface/EventSetup.h" 
#include "FWCore/Framework/interface/ESHandle.h" 
#include "DataFormats/HepMCCandidate/interface/GenParticle.h" //to work with reco::GenParticle
#include "DataFormats/PatCandidates/interface/Jet.h" //to work with pat::Jets
#include "DataFormats/PatCandidates/interface/PackedCandidate.h" //to work with particles inside jets
#include <vector> 
#include <string> 
#include <map>
#include <iostream>
#include <fstream>
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Run.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "FWCore/ServiceRegistry/interface/Service.h" // to use TFileService
#include "CommonTools/UtilAlgos/interface/TFileService.h" // to use TFileService


//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.


class FastJetSimple1 : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit FastJetSimple1(const edm::ParameterSet&);
      ~FastJetSimple1();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      // ----------member data ---------------------------
//      edm::EDGetTokenT<TrackCollection> tracksToken_;  //used to select what tracks to read from configuration file
	edm::EDGetTokenT<std::vector<pat::Muon>> patmuonToken;
//	edm::EDGetTokenT<std::vector<pat::IsolatedTrack>> tracksToken;
	edm::Service<TFileService> fs;



	TH1F *hist_genpt; 
	TH1F *hist_m1pt; 
	TH1F *hist_m2pt; 	
	TH1F *hist_m3pt;  
	TH1F *hist_m4pt;
	TH1I *hist_muonsnumber;

};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
FastJetSimple1::FastJetSimple1(const edm::ParameterSet& iConfig)
 :
//  tracksToken_(consumes<TrackCollection>(iConfig.getUntrackedParameter<edm::InputTag>("tracks")))
patmuonToken(consumes<std::vector<pat::Muon> >(iConfig.getUntrackedParameter<edm::InputTag>("muonpat")))

{
   //now do what ever initialization is needed
	hist_genpt = fs->make<TH1F>("Muonpt", "High pt muons", 200, 0.0, 200.0);
	hist_m1pt = fs->make<TH1F>("Muon1pt", "First muon pt", 100, 0.0, 100.0);
	hist_m2pt = fs->make<TH1F>("Muon2pt", "Second muon pt", 100, 0.0, 100.0);
	hist_m3pt = fs->make<TH1F>("Muon3pt", "Third muon pt", 100, 0.0, 100.0);
	hist_m4pt = fs->make<TH1F>("Muon4pt", "Muon pt", 100, 0.0, 100.0);
	hist_muonsnumber = fs->make<TH1I>("Muonsnumber", "number of reconstructed muons", 20, -0.5, 19.5);


}


FastJetSimple1::~FastJetSimple1()
{

   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
FastJetSimple1::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
	edm::Handle<std::vector<pat::Muon> > patmuon;
	iEvent.getByToken(patmuonToken, patmuon);

	std::vector<float> m_pt;
	std::vector<float> m_eta;
	std::vector<float> m_phi;
	int m_number = 0;
		for (std::vector<pat::Muon>::const_iterator itGen=patmuon->begin(); itGen!=patmuon->end(); ++itGen) 
		{
		float pt = itGen->pt();
		m_pt.push_back(pt);
		m_phi.push_back(itGen->phi());
		m_eta.push_back(itGen->eta());
		m_number=m_number+1;
    		}

	hist_muonsnumber->Fill(m_number);
	std::sort(m_pt.begin(), m_pt.end()); 
	std::reverse(m_pt.begin(), m_pt.end());

    for (unsigned int i=0; i<m_pt.size() && i<4; i++){
	      hist_genpt->Fill(m_pt[i]);
    }
    if (m_pt.size() >3) {hist_m1pt->Fill(m_pt[0]); hist_m2pt->Fill(m_pt[1]); hist_m3pt->Fill(m_pt[2]); hist_m4pt->Fill(m_pt[3]);}
    else if (m_pt.size()==3) {hist_m1pt->Fill(m_pt[0]); hist_m2pt->Fill(m_pt[1]); hist_m3pt->Fill(m_pt[2]); hist_m4pt->Fill(0);}
    else if (m_pt.size()==2) {hist_m1pt->Fill(m_pt[0]); hist_m2pt->Fill(m_pt[1]); hist_m3pt->Fill(0); hist_m4pt->Fill(0);}
    else if (m_pt.size()==1) {hist_m1pt->Fill(m_pt[0]); hist_m2pt->Fill(0); hist_m3pt->Fill(0); hist_m4pt->Fill(0);}


/*
    for(TrackCollection::const_iterator itTrack = tracks->begin();
        itTrack != tracks->end();
        ++itTrack) {
      // do something with track parameters, e.g, plot the charge.
      // int charge = itTrack->charge();
    }
*/




#ifdef THIS_IS_AN_EVENT_EXAMPLE
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);
#endif

#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
#endif
}


// ------------ method called once each job just before starting event loop  ------------
void
FastJetSimple1::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
FastJetSimple1::endJob()
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
FastJetSimple1::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);

  //Specify that only 'tracks' is allowed
  //To use, remove the default given above and uncomment below
  //ParameterSetDescription desc;
  //desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
  //descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(FastJetSimple1);
