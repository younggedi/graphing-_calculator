#include "graph_logic/graph.hpp"
int main()
{
    int no_of_equation=1; 
    cout <<"Enter an number equation"<<endl;
    cin>>no_of_equation; 
    vector<vector<string>> example;
    vector <int> bracket_counter;
    int x;
    vector<string>color_list;
    for(int i=0;i<no_of_equation;i++){
        int bracket=0;
        string equation,colors;
        cout <<"Enter an equation "<<(i+1)<<endl;
        cin>>ws;
        getline(cin, equation);
        cout <<"Enter your color for equation "<<(i+1)<<endl;
        cin>>colors;
        color_list.push_back(colors);
        vector<string> subexample;
        subexample=parser(equation,bracket);
        example.push_back(subexample);
        bracket_counter.push_back(bracket);
    }
    
    double start=0;
    double stop=1;
    double step=0.01;
    cout<<"enter your start: "<<endl;
    cin>>start;
    cout<<"enter your stop:"<<endl;
    cin>>stop;
    cout<<"enter your step:"<<endl;
    cin>>step;
    Node* eq=NULL;
    vector<vector<double>> yvalues;
    vector<vector<double>> xvalues;
    int i=0;
    for(int no=0;no<no_of_equation;no++){
        eq=build_ast(example[no],start,i);
        vector<double> subyvalues;
        vector<double> subxvalues;
        for(double k=start;k<=stop;k+=step){
            for(int j=0;j<=bracket_counter[no]+1;j++){
                try_solve(eq,k);
        }
        //if(!(std::isfinite(eq->result))){
        //  if(no<no_of_equation-1)no++;
        // continue;
        //}
            subyvalues.push_back(eq->result);
            subxvalues.push_back(k);
        }
        yvalues.push_back(subyvalues);
        xvalues.push_back(subxvalues);
        free_ast(eq);
    }

    sf::RenderWindow window( sf::VideoMode( { SCREEN_WIDTH, SCREEN_HEIGHT } ), "SFMLstuff!" );
	while ( window.isOpen() )
	{
		while ( const std::optional event = window.pollEvent() )
		{
			if ( event->is<sf::Event::Closed>() )
				window.close();
		}

        window.clear();
        DrawLine(originX,0,originX,SCREEN_HEIGHT,"white",window);
        DrawLine(0,originY,SCREEN_WIDTH,originY,"white",window);
        for(int no=0;no<no_of_equation;no++){
            for(int i=0;i<xvalues[no].size()-1;i++){
                Drawminiline(xvalues[no][i],yvalues[no][i],xvalues[no][i+1],yvalues[no][i+1],color_list[no],window,50,20);
            }
        }
		window.display();
	}
	
}
