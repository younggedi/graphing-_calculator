#include "graph.hpp"
int main()
{
    string equation;
    cout <<"Enter an equation"<<endl;
    getline(cin, equation);
    int x,bracket_counter=0;
    vector<string> example= parser(equation,bracket_counter);
    double start=0;
    double stop=1;
    double step=0.05;
    cout<<"enter your start: "<<endl;
    cin>>start;
    cout<<"enter your stop:"<<endl;
    cin>>stop;
    Node* eq=NULL;
    int i=0;
    vector<double> yvalues;
    vector<double> xvalues;
    eq=build_ast(example,start,i);
    for(double k=start;k<=stop;k+=step){
    for(int j=0;j<=bracket_counter+1;j++){
        try_solve(eq,k);
        resolve_brackets(eq);
        resolve_keywords(eq);
    }
     cout<<"x="<<k<<" y="<<eq->result<<endl;
     yvalues.push_back(eq->result);
     xvalues.push_back(k);
    }
    free(eq);
    int totalx=yvalues.size();
    auto [min_y,max_y]=minmax(yvalues.begin(),yvalues.end());
    double diff=max_y-min_y;
    double scaley=SCREEN_HEIGHT/diff;
    double scalex=SCREEN_WIDTH/totalx;
    sf::RenderWindow window( sf::VideoMode( { 2560, 1440 } ), "SFMLstuff!" );
	//sf::CircleShape shape( 250.f );
	//shape.setFillColor( sf::Color::Yellow );
    //shape.setRadius(5.0);
	while ( window.isOpen() )
	{
        
        //auto [width,height]=shape.getPosition();
        //auto [wwidth,wheight]=window.getSize();
        //if(2560>width)shape.setPosition({width+1,height+1});
        //if(2560<width)shape.setPosition({width-1,height});
		while ( const std::optional event = window.pollEvent() )
		{
			if ( event->is<sf::Event::Closed>() )
				window.close();
            //if ( event->is<sf::Event::KeyPressed>() )
                //shape.setFillColor(sf::Color::Blue);
		}

        window.clear();
        DrawLine(originX,0,originX,SCREEN_HEIGHT,sf::Color::White,window);
        DrawLine(0,originY,SCREEN_WIDTH,originY,sf::Color::White,window);
        for(int i=0;i<totalx-1;i++){
            Drawminiline(xvalues[i],yvalues[i],xvalues[i+1],yvalues[i+1],sf::Color::Yellow,window,50,50);
        }
		window.display();
	}
	
}
